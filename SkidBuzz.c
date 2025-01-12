#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <math.h>

// Constants
#define MAX_DIGITS 5 // Allow up to 99999
#define FONT_HEIGHT 10
#define SCROLL_STEP 1

typedef enum {
    SkidBuzzModeInputNumber,
    SkidBuzzModeViewResult,
    SkidBuzzModeMainMenu,
    SkidBuzzModeInstructions
} SkidBuzzAppMode;

typedef struct {
    char user_input[MAX_DIGITS + 1]; // User-entered number as a string
    int input_length;               // Current input length
    char result[32];                // Precomputed SkidBuzz result
    SkidBuzzAppMode mode;           // Current app mode
    uint8_t horizontal_scroll_offset; // For scrolling long results
    bool running;                   // Flag to control the main loop
} SkidBuzzApp;

// Function to compute the SkidBuzz result for a given number
static void compute_skidbuzz(int number, SkidBuzzApp* app) {
    if (number % 15 == 0) {
        snprintf(app->result, sizeof(app->result), "%d: SkidBuzz", number);
    } else if (number % 5 == 0) {
        snprintf(app->result, sizeof(app->result), "%d: Skid", number);
    } else if (number % 3 == 0) {
        snprintf(app->result, sizeof(app->result), "%d: Buzz", number);
    } else {
        snprintf(app->result, sizeof(app->result), "%d", number);
    }
}

// Input callback to handle user interactions
static void skidbuzz_input_callback(InputEvent* input_event, void* context) {
    SkidBuzzApp* app = context;

    if (input_event->type == InputTypePress) {
        switch (app->mode) {
            case SkidBuzzModeMainMenu:
                switch (input_event->key) {
                    case InputKeyOk:
                        app->mode = SkidBuzzModeInputNumber; // Transition to input mode
                        app->input_length = 0; // Reset input length
                        app->user_input[0] = '\0'; // Clear previous input
                        break;
                    case InputKeyBack:
                        app->running = false; // Exit the app
                        break;
                    default:
                        break;
                }
                break;

            case SkidBuzzModeInputNumber:
                switch (input_event->key) {
                    case InputKeyUp:
                        if (app->input_length > 0) {
                            char current_digit = app->user_input[app->input_length - 1];
                            app->user_input[app->input_length - 1] = ((current_digit - '0' + 1) % 10) + '0';
                        }
                        break;
                    case InputKeyDown:
                        if (app->input_length > 0) {
                            char current_digit = app->user_input[app->input_length - 1];
                            app->user_input[app->input_length - 1] = ((current_digit - '0' + 9) % 10) + '0'; // Decrement digit
                        }
                        break;
                    case InputKeyRight:
                        if (app->input_length < MAX_DIGITS) {
                            app->user_input[app->input_length++] = '0';
                            app->user_input[app->input_length] = '\0';
                        }
                        break;
                    case InputKeyLeft:
                        if (app->input_length > 0) {
                            app->user_input[--app->input_length] = '\0';
                        }
                        break;
                    case InputKeyOk:
                        if (app->input_length > 0) {
                            int number = atoi(app->user_input); // Convert input to integer
                            compute_skidbuzz(number, app); // Compute result
                            app->mode = SkidBuzzModeViewResult; // Transition to result mode
                        }
                        break;
                    case InputKeyBack:
                        app->mode = SkidBuzzModeMainMenu; // Return to main menu
                        break;
                    default:
                        break;
                }
                break;

            case SkidBuzzModeViewResult:
                switch (input_event->key) {
                    case InputKeyOk:
                    case InputKeyBack:
                        app->mode = SkidBuzzModeMainMenu; // Return to main menu
                        break;
                    case InputKeyLeft:
                        if (app->horizontal_scroll_offset > 0) {
                            app->horizontal_scroll_offset -= SCROLL_STEP;
                        }
                        break;
                    case InputKeyRight:
                        app->horizontal_scroll_offset += SCROLL_STEP;
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

// Render callback to display the UI
static void skidbuzz_render_callback(Canvas* canvas, void* context) {
    SkidBuzzApp* app = context;

    canvas_clear(canvas); // Clear the canvas

    switch (app->mode) {
        case SkidBuzzModeMainMenu:
            canvas_draw_str(canvas, 10, 10, "SkidBuzz Main Menu:");
            canvas_draw_str(canvas, 10, 30, "OK: Enter Number");
            canvas_draw_str(canvas, 10, 50, "BACK: Exit");
            break;

        case SkidBuzzModeInputNumber:
            canvas_draw_str(canvas, 10, 10, "Enter a number:");
            canvas_draw_str(canvas, 10, 30, app->user_input[0] ? app->user_input : "0");
            canvas_draw_str(canvas, 10, 50, "OK: Compute BACK: Cancel");
            break;

        case SkidBuzzModeViewResult:
            canvas_draw_str(canvas, 10, 10, "Result:");
            canvas_draw_str(canvas, 10, 30, app->result + app->horizontal_scroll_offset);
            canvas_draw_str(canvas, 10, 50, "OK: Back LEFT/RIGHT: Scroll");
            break;

        default:
            canvas_draw_str(canvas, 10, 10, "Unexpected mode!");
            break;
    }
}

// Main function for the SkidBuzz app
int32_t skidbuzz_main(void* p) {
    (void)p;

    // Allocate and initialize the app structure
    SkidBuzzApp* app = malloc(sizeof(SkidBuzzApp));
    app->mode = SkidBuzzModeMainMenu;
    app->input_length = 0;
    app->user_input[0] = '\0';
    app->result[0] = '\0';
    app->horizontal_scroll_offset = 0;
    app->running = true;

    // Set up the GUI
    ViewPort* viewport = view_port_alloc();
    view_port_draw_callback_set(viewport, skidbuzz_render_callback, app);
    view_port_input_callback_set(viewport, skidbuzz_input_callback, app);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, viewport, GuiLayerFullscreen);

    // Main loop
    while (app->running) {
        furi_delay_ms(100);
    }

    // Cleanup
    gui_remove_view_port(gui, viewport);
    view_port_free(viewport);
    furi_record_close("gui");
    free(app);

    return 0;
}
