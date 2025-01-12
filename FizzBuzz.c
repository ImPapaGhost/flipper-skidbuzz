#include <furi.h>
#include <gui/gui.h>

// Constants
#define MAX_COUNT 100

// Global array to store precomputed FizzBuzz results
static char fizzbuzz_results[MAX_COUNT][32];

// Precompute FizzBuzz results during setup
void precompute_fizzbuzz() {
    for(int i = 1; i <= MAX_COUNT; i++) {
        if(i % 15 == 0) {
            snprintf(fizzbuzz_results[i - 1], sizeof(fizzbuzz_results[i - 1]), "%d: FizzBuzz", i);
        } else if(i % 5 == 0) {
            snprintf(fizzbuzz_results[i - 1], sizeof(fizzbuzz_results[i - 1]), "%d: Fizz", i);
        } else if(i % 3 == 0) {
            snprintf(fizzbuzz_results[i - 1], sizeof(fizzbuzz_results[i - 1]), "%d: Buzz", i);
        } else {
            snprintf(fizzbuzz_results[i - 1], sizeof(fizzbuzz_results[i - 1]), "%d", i);
        }
    }
}

// Callback function to display the current result
void fizzbuzz_callback(Canvas* canvas, void* ctx) {
    (void)ctx; // Mark ctx as unused
    static int index = 0;

    // Clear the canvas
    canvas_clear(canvas);

    // Draw the precomputed FizzBuzz result for the current index
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 10, 30, fizzbuzz_results[index]);

    // Increment the index, looping back to 0 after reaching MAX_COUNT
    index = (index + 1) % MAX_COUNT;
}

int32_t fizzbuzz_main(void* p) {
    (void)p; // Mark p as unused

    // Precompute FizzBuzz results
    precompute_fizzbuzz();

    // Set up GUI and viewport
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* viewport = view_port_alloc();

    // Assign the callback for the viewport
    view_port_draw_callback_set(viewport, fizzbuzz_callback, NULL);

    // Add the viewport to the GUI
    gui_add_view_port(gui, viewport, GuiLayerFullscreen);

    // Keep the application running, updating every second
    for(int i = 0; i < MAX_COUNT; i++) {
        furi_delay_ms(1000); // 1-second delay between updates
    }

    // Clean up resources
    view_port_free(viewport);
    furi_record_close(RECORD_GUI);

    return 0;
}
