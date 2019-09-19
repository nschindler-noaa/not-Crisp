/** \file
 *
 *	\brief Contains the structure to be used in configuring a specific instance
 * of the schedule frame.
 */

struct schedule_button {
    char *button_label;
    void (*button_proc)();
    int button_x;
    int button_y;
    char button_exit_frame;
    void *button_data;
    struct schedule_button *next;

    /* This should not be set by the calling routine.  Internal use only.
     */
    Panel_item button_item;
};

struct schedule_data {
    /* These should be set by the calling routine */
    int per_min_val;
    int per_max_val;
    float per_val_min_val;
    float per_val_max_val;
    int per_min_def;
    int per_max_def;
    PERIOD_LIST **periods;
    char *value_title;
    char *list_title;
    char *hist_name;
    void *user_data;
    struct schedule_button *buttons;
    void (*run_func)();

    /* These should not be set by the calling routine -- they're used
     * internally only
     */
    Frame	period_frame;
    Panel_item	list_item;
    Panel_item	create_item;
    Panel_item	modify_item;
};

