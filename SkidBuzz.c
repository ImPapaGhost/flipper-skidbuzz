#include <furi.h>
#include <gui/gui.h>

// Constants
#define MAX_COUNT 100

// Global array to store precomputed SkidBuzz results
static char skidbuzz_results[MAX_COUNT][32];

// Precompute SkidBuzz results during setup
void precompute_skidbuzz() {
    for(int i = 1; i <= MAX_COUNT; i++) {
        if(i % 15 == 0) {
            snprintf(skidbuzz_results[i - 1], sizeof(skidbuzz_results[i - 1]), "%d: SkidBuzz", i);
        } else if(i % 5 == 0) {
            snprintf(skidbuzz_results[i - 1], sizeof(skidbuzz_results[i - 1]), "%d: Skid", i);
        } else if(i % 3 == 0) {
            snprintf(skidbuzz_results[i - 1], sizeof(skidbuzz_results[i - 1]), "%d: Buzz", i);
        } else {
            snprintf(skidbuzz_results[i - 1], sizeof(skidbuzz_results[i - 1]), "%d", i);
        }
    }
}

// Callback function to display the current result
void skidbuzz_callback(Canvas* canvas, void* ctx) {
    (void)ctx; // Mark ctx as unused
    static int current_index = 0;

    // Clear the canvas
    canvas_clear(canvas);

    // Draw the precomputed SkidBuzz result for the current index
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 10, 30, skidbuzz_results[current_index]);

    // Increment the index, looping back to 0 after reaching MAX_COUNT
    current_index = (current_index + 1) % MAX_COUNT;
}

int32_t skidbuzz_main(void* p) {
    (void)p; // Mark p as unused

    // Precompute SkidBuzz results
    precompute_skidbuzz();

    // Set up GUI and viewport
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* viewport = view_port_alloc();

    // Assign the callback for the viewport
    view_port_draw_callback_set(viewport, skidbuzz_callback, NULL);

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
