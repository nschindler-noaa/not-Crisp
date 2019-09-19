/* \file
 * \brief Externals include file for filer module */

/* pointers to functions to actually read and write the parameter data.
 * These are of the form: 
 *	 int	(*fn) (file_descriptor)
 *		  int file_descriptor;
 */

extern int (*read_param_data) ();		/* function to read data */
extern int (*write_param_data) ();		/* function to write data */

/* pointers to functions to actually read and write the data.
 * These are of the form: 
 *	 int	(*fn) (file)
 *		  FILE file;
 */

int	(*output_all_data) ();		/* pointer to function to write data */
int	(*output_all_run_data) ();	/* pointer to function to write run data */
int	(*output_all_spec_data) ();	/* pointer to function to write species data */

/* functions exported by the parameter window module */
extern void set_filer_window();
extern void show_filer_window();
extern void kill_filer_window();

