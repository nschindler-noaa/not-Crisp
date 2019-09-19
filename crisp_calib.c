/** \file
 * \brief crisp_calib tool (see \ref crisp_calib). Not part of COMPASS GUI or batch mode! (see \ref crisp_calib)
 *
 * \section crisp_calib CRiSP1 Calibration Tool
 *
 * This is a stand-alone program used to invoke crisp1 with a specific
 * set of calibration parameters.  In a typical use, it is called
 * repeatedly from a top-level calibration program, which specifies a
 * different set of parameters for each invocation, and analyzes the
 * resulting crisp output datafile at each step.  The calling program
 * is responsible for performing the data extraction from the crisp
 * output file.  The file "crisp_extract.c" contains a routine
 * suitable for this purpose.
 *
 * The basic working of the program is as follows:
 *
 * 1) Using the information in the crisp_calib input data file, a
 * crisp1 release data file is generated.  The release data file will
 * contain an independent release for each "release date" specified in
 * the input data file (see below).  Each release will be tagged
 * using the crisp1 "rt_stock" token, which will be generated as the
 * stock name followed by an underscore followed by the date of the
 * release (e.g. "Snaketrap_Ch1_107").  It is necessary to use this as
 * an indentifier when performing an extraction.  The "crisp_extract"
 * routine mentioned above is configured to generate this type of
 * release identifier from a stock name and original release date.
 *
 * 2) The user also specifies a crisp input data file to use, and an
 * awk script to be run on the specified datafile.  The user may
 * optionally specify a sed command to first run on the awk script
 * itself, and a string constituting the command line parameters for
 * the awk invocation.  A final awk script is generated, which is then
 * used with the crisp input file to create a new crisp input file
 * with the parameters appropriate to the current run.  Finally, the
 * release file generated in step 1 is "included" in the new crisp
 * data file.
 *
 * 3) The crisp1 executable is invoked, specifying the new input data
 * file created by the awk script in step 2.
 *
 * Extraction of result data is not performed.  crisp1 is invoked in
 * "realtime" mode, and the resulting realtime raw output data file is
 * left in its default location for examination by the user or calling
 * program.
 *
 * The user may specify the name of the crisp_calib input data file on
 * the command line.  If not specified, the default input file (see
 * "crisp_calib.h" for name) is used.  The crisp_calib input data file
 * consists of a series of token/value pairs.  The tokens are
 * specified in the "crisp_calib.h" header file.
 *
 * Crisp_calib input data tokens: A new release record is specified
 * with the "release_site" token.  This must appear prior to other
 * release information.  The "stock" and "species" tokens specify
 * actual stock and species names found in the crisp1 data file
 * corresponding to the release currently being specified.  The
 * "release_site" must also specify a release named in the crisp1
 * river description file ("columbia.desc" by default).  "smolt_onset"
 * and "smolt_finish" specify the values of those crisp variables.
 * The "rls_date:length:number" specifies the dates on which to
 * release a generic number of fish, an average length for each, and
 * the number to release.  If the length given is 0, then no
 * length-dependent calculations occur in crisp.  The number of dates
 * is given as part of the value of this token.  Note that these will
 * be created as fully independent point releases, and will not be
 * merged on output.
 *
 * Here are the contents of a sample "crisp_calib.input" file:
 *
 *	crisp_datafile 97.dat 
 *	awk_script calibration.awk 
 *	awk_sed_cmd '/stockname/s//Snaketrap_Ch1/' 
 *	awk_parameters Bmin=3.5 Bmax=2.0 
 *	release_site Wild_Snake_R_Stocks 
 *	stock Snaketrap_Ch1 
 *	species Steelhead 
 *	smolt_onset 80 
 *	smolt_finish 80 
 *	rls_date:length:number (3) 
 *		107:150.0:10000 
 *		136:99.0:10000 
 *		137:145.2:10000  
 * */

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <netdb.h>
#include <time.h>
#include "crisp_calib.h"
#include "version.h"

int process_file();
CrispCalib *read_calib();
int read_calib_data();
ReleaseInfo* create_new_release();
int read_rls_dates();
int read_tt_params();
SplitLine *split_input_line();
char *read_line();
char *get_next_token();
SplitLine *split_line();
char *get_line_remainder();
void remove_line_feeds();
void free_split_line();
void free_calib_data();
int write_calib_rls();
int write_crisp_header();
int write_release_data();
int check_crisp_calib();
/* cmd line option:  =1 indicates that continuos time should be used in Crisp.
 * Typically used for travel time calib */
extern int tt_flag = 0; 

int main(argc, argv)
	int argc;
	char *argv[];
{
    extern int		stat();
    extern void		perror();

    char		*input_filename	 = DFLT_INPUT_FILE;
    struct stat		sbuf;

    static char *usage = "crisp_calib version " COMPASS_VERSION_MAJOR_STRING "." COMPASS_VERSION_MINOR_STRING "\n"
	"Usage: crisp_calib [-i input_file] [-t <travel time calib flag>]";
    extern int getopt();
    extern char *optarg;
    int c;

    while ((c = getopt(argc, argv, "i:t")) != -1) {
	switch (c) {
	  case 'i':
	    {
		char *path = optarg;
		if (stat(path, &sbuf) < 0) {
		    perror("stat");
		    path = input_filename;
		}
		input_filename = path;
		break;
	    }
	  case 't':
	    {
		tt_flag = 1;
		break;
	    }
	  case '?':
	  default:
	    fprintf(stderr, "%s\n", usage);
	    return(1);
	}
    }

    return process_file(input_filename);
}

	
int process_file(input_path)
    char	*input_path;
{
    CrispCalib		*calib;
    char		*output_path;
    
    output_path = ".";

    if (access(output_path, W_OK) < 0) {
	fprintf(stderr, "Can't write to output directory \"%s\" (%s)\n",
		output_path, strerror(errno));
	return(1);
    }

    if (!(calib = read_calib(input_path))) {
	fprintf(stderr, "Error reading calib data for %s\n",
		input_path);
    }
    if (check_crisp_calib(calib) != 0) {
	free_calib_data(calib);
	fprintf(stderr, "Inconsistent or missing calib data.\n");
	return(1);
    }
    if (write_calib_dat(calib) != 0) {
	free_calib_data(calib);
	fprintf(stderr, "Fatal error writing dat file.\n");
	return(1);
    }
    if (write_calib_rls(calib->releases) < 0) {
	free_calib_data(calib);
	fprintf(stderr, "Fatal error writing rls file.\n");
	return(1);
    }
    if (run_crisp(calib) != 0) {
	free_calib_data(calib);
	fprintf(stderr, "Fatal error running crisp.\n");
	return(1);
    }
    free_calib_data(calib);
    return(0);
}


CrispCalib *read_calib(input_filename)
    char *input_filename;
{
    FILE *ifp;
    CrispCalib *data = 0;

    if ((ifp = fopen(input_filename, "r")) == (FILE *) 0) {
	fprintf(stderr, "Can't open file \"%s\"!\n", input_filename);
	return 0;
    }
    if (!(data = (CrispCalib *) calloc(1, sizeof(CrispCalib)))) {
	fprintf(stderr, "Out of virtual memory!\n");
	return 0;
    }
    if (read_calib_data(data, ifp) < 0) {
	fclose(ifp);
	free_calib_data(data);
	return 0;
    }
    fclose(ifp);
    return data;
}

int read_calib_data(calib, ifp)
	CrispCalib	*calib;
	FILE		*ifp;
{
    SplitLine *line=0;
    int error_occurred = 0;
    ReleaseInfo *current_release = 0;

    while((line = split_input_line(ifp)) && !error_occurred) {
	if (line->token_count > 0) {
	    /* general information */
	    if (!strcmp(line->line_tokens[0], CC_CRISP_DATA)) {
		calib->datafile = strdup(line->line_tokens[1]);
		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_RIVER_DESC)) {
		calib->river_description = strdup(line->line_tokens[1]);
		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_AWK_SCRIPT)) {
		calib->awk_script = strdup(line->line_tokens[1]);
		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_AWK_PARAMETERS)) {
		calib->awk_parameters = strdup(get_line_remainder(line, 1));
		remove_line_feeds(calib->awk_parameters);
		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_AWK_SED_CMD)) {
		calib->awk_sed_cmd = strdup(get_line_remainder(line, 1));
		remove_line_feeds(calib->awk_sed_cmd);
		free_split_line(line);
	    }

	    /* release information */
	    else if (!strcmp(line->line_tokens[0], CC_SITE)) {
		/* a release site token indicates a new set of release data */
		char *site = strdup(line->line_tokens[1]);
		current_release = create_new_release(calib, site);
		if (!current_release)
		    ++error_occurred;
		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_STOCK)) {
		char* stock = strdup(line->line_tokens[1]);
		if (!current_release) {
		    fprintf(stderr, "Error: no release site specified for stock %s in calibration file.\n", stock);
		    ++error_occurred;
		}
		else
		    current_release->stock = stock;

		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_SPECIES)) {
		char* species = strdup(line->line_tokens[1]);
		if (!current_release) {
		    fprintf(stderr, "Error: no release site specified for species %s in calibration file.\n", species);
		    ++error_occurred;
		}
		else
		    current_release->species = species;

		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_SMOLT_ONSET)) {
		if (!current_release) {
		    fprintf(stderr, "Error: no release site specified for smolt onset data in calibration file.\n");
		    ++error_occurred;
		}
		else
		    sscanf(line->line_tokens[1], "%d", &current_release->smolt_onset);

		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_SMOLT_FINISH)) {
		if (!current_release) {
		    fprintf(stderr, "Error: no release site specified for smolt finish data in calibration file.\n");
		    ++error_occurred;
		}
		else
		    sscanf(line->line_tokens[1], "%d", &current_release->smolt_finish);

		free_split_line(line);
	    }
	    else if (!strcmp(line->line_tokens[0], CC_RLS_DATES))
		error_occurred += read_rls_dates(current_release, ifp, line);
	    else {
		fprintf(stderr, "Error: unrecognized line in input file!\n\"%s\"\n",line->original_line);
		free_split_line(line);
		++error_occurred;
	    }
	}
    }
    return error_occurred;
}

/* create a new release info record and add it to the list */

ReleaseInfo* create_new_release(CrispCalib* calib, char* site)
{
    ReleaseInfo* release = (ReleaseInfo*) calloc(1, sizeof(ReleaseInfo));
    if (release) {
	release->site = site;
	if (calib->releases) {
	    ReleaseInfo* last_release = calib->releases;
	    while (last_release->next)
		last_release = last_release->next;
	    last_release->next = release;
	}
	else
	    calib->releases = release;
    }
    return release;
}

int read_rls_dates(release, ifp, line)
	ReleaseInfo 	*release;
	FILE		*ifp;
	SplitLine	*line;
{
    int return_code = 0;
    int t_indx, i, num_dates=0;
    
    if (line->token_count < 2
	|| (release->rls_dates) 	/* release is supposed to be blank */
	|| (sscanf(line->line_tokens[1], "(%d)", &num_dates) != 1)
	|| !num_dates)
	return_code = 1;

    else {
	/* make null-terminated array of release dates */
	release->rls_dates = (int *)malloc((num_dates+1)*sizeof(int));
	release->rls_lengths = (float *)malloc((num_dates+1)*sizeof(float));
	release->rls_numbers = (float *)malloc((num_dates+1)*sizeof(float));
	if (!release->rls_dates || !release->rls_lengths || !release->rls_numbers) {
	    fprintf(stderr, "Error: out of memory!\n");
	    return 1;
	}
	bzero((char *)release->rls_dates, (num_dates+1)*sizeof(int));
	bzero((char *)release->rls_lengths, (num_dates+1)*sizeof(float));
	bzero((char *)release->rls_numbers, (num_dates+1)*sizeof(float));
	t_indx = 2;
	for (i=0; i < num_dates; ++i) {
	    char *token = get_next_token(&line, ifp, &t_indx);
	    if (!token || (sscanf(token, "%d:%f:%f",
				  &release->rls_dates[i],
				  &release->rls_lengths[i],
				  &release->rls_numbers[i]) != 3)) {
		return_code = 1;
		break;
	    }
	}
    }
    /* sanity check */
    if (t_indx != line->token_count)
	return_code = 1;

    free_split_line(line);

    if (return_code)
	fprintf(stderr, "Error reading rls dates (number problem?)!\n");
    return return_code;
}

int save_backup(output_filename)
	char *output_filename;
{
    if (access(output_filename, F_OK) == 0) {
	char tilde_filename[1024];

	sprintf(tilde_filename, "%s~", output_filename);
	if (access(tilde_filename, F_OK) == 0) {
	    if (unlink(tilde_filename) < 0) {
		fprintf(stderr, "Can't remove old backup file!\n");
		return -1;
	    }
	}
	if (rename(output_filename, tilde_filename) < 0) {
	    fprintf(stderr, "Can't create backup file!\n");
	    return -1;
	}
    } else
	return 1;
    return 0;
}

SplitLine *split_input_line(ifp)
    FILE *ifp;
{
    return split_line(read_line(ifp));
}

char *read_line(ifp)
	FILE	*ifp;
{
    extern char *fgets();

    static char input_buffer[1024];

    if (!(fgets(input_buffer, sizeof(input_buffer)-1, ifp)))
	return 0;
    input_buffer[sizeof(input_buffer)-1] = 0;
    return input_buffer;
}

char *get_next_token(linep, ifp, t_indx)
    SplitLine **linep;
    FILE *ifp;
    int *t_indx;
{
    SplitLine *line = *linep;
    if (*t_indx >= line->token_count) {
	free_split_line(line);
	line = split_input_line(ifp);
	*linep = line;
	*t_indx = 0;
	if (!line)
	    return 0;
	if (line->token_count == 0) 
	    return get_next_token(linep, ifp, t_indx);
    }
    return line->line_tokens[(*t_indx)++];
}

/* return the remainder of the line beginning with a certain token index.
 * returns a pointer into the "original_line" field of the split line.
 */

char* get_line_remainder(SplitLine* line, int token)
{
    char* remainder = line->original_line;
    int cur_tok = 0;
    /* if requested token is too big return pointer to the
       null-terminator */
    if (token >= line->token_count)
	return line->original_line+strlen(line->original_line);
    for (cur_tok = 0; cur_tok <= token; ++cur_tok) {
	remainder = strstr(remainder, line->line_tokens[cur_tok]);
    }
    return remainder;
}

void remove_line_feeds(char* str)
{
    char* ptr;
    while (ptr=strchr(str, '\n'))
	*ptr = ' ';
}

SplitLine *split_line(line)
	char	*line;
{
    SplitLine	*new_line;
    char	**tokens;
    unsigned	number_tokens = 15;
    char	*ptr;

    if (!line)
	return 0;
    if (!(new_line = (SplitLine *) malloc(sizeof(SplitLine)))) {
	fprintf(stderr, "Out of virtual memory!\n");
	return 0;
    }
    bzero(new_line, sizeof(SplitLine));
    if (!(new_line->original_line = strdup(line))) {
	fprintf(stderr, "Out of virtual memory!\n");
	free(new_line);
	return 0;
    }
    if (!(tokens = (char **) malloc((number_tokens+1) * sizeof(char *)))) {
	fprintf(stderr, "Out of virtual memory!\n");
	free(new_line->original_line);
	free(new_line);
	return 0;
    }
    if ((ptr = strtok(line, " \t\n")) && *ptr)
	do {
	    if (!(tokens[new_line->token_count] = strdup(ptr))) {
		fprintf(stderr, "Out of virtual memory!\n");
		while (new_line->token_count--)
		    free(tokens[new_line->token_count]);
		free(tokens);
		free(new_line->original_line);
		free(new_line);
		return 0;
	    }
	    ptr = strtok((char *) 0, " \t\n");
	    if ((++new_line->token_count == number_tokens) && ptr) {
		number_tokens *= 2;
		if (!(tokens = (char **)
		      realloc(tokens, (number_tokens+1) * sizeof(char *)))) {
		    fprintf(stderr, "Out of virtual memory!\n");
		    while (new_line->token_count--)
			free(tokens[new_line->token_count]);
		    free(tokens);
		    free(new_line->original_line);
		    free(new_line);
		    return 0;
		}
	    }
	} while (ptr);
    tokens[new_line->token_count] = 0;
    new_line->line_tokens = (char **) realloc(tokens,
		(new_line->token_count + 1) * sizeof(char *));
    return new_line;
}

void free_split_line(data)
	SplitLine	*data;
{
    if (data) {
	if (data->original_line)
	    free(data->original_line);
	if (data->line_tokens) {
	    while (data->token_count--)
		if (data->line_tokens[data->token_count])
		    free(data->line_tokens[data->token_count]);
	    free(data->line_tokens);
	}
	free(data);
    }
}

void free_release_info(ReleaseInfo* release)
{
    if (release) {
	free_release_info(release->next);
	if (release->site) free(release->site);
	if (release->species) free(release->species);
	if (release->stock) free(release->stock);
	if (release->rls_dates) free(release->rls_dates);
	if (release->rls_lengths) free(release->rls_lengths);
	if (release->rls_numbers) free(release->rls_numbers);

	free(release);
    }
}

void free_calib_data(data)
	CrispCalib	*data;
{
    if (data) {
	if (data->datafile) free(data->datafile);
	if (data->awk_script) free(data->awk_script);
	if (data->awk_parameters) free(data->awk_parameters);
	if (data->awk_sed_cmd) free(data->awk_sed_cmd);

	if (data->releases) 
	    free_release_info(data->releases);

	free(data);
    }
}

int check_crisp_releases(ReleaseInfo* release)
{
    if (release) {
	if (check_crisp_releases(release->next)
	|| !release->site
	|| !release->species
	|| !release->stock
	|| !release->rls_dates
	|| !release->rls_lengths
	|| !release->rls_numbers
	)
	    return -1;
    }
    return 0;
}

int check_crisp_calib(calib)
    CrispCalib *calib;
{
    if (!calib || !calib->datafile || check_crisp_releases(calib->releases))
	return -1;
    else
	return 0;
}
	
int write_calib_rls(ReleaseInfo* releases)
{
    FILE	*ofp;
    static int	wrote_header = 0;
    static char *output_filename = CRISP_RLSFILE;

    if (save_backup(output_filename) < 0)
	return -1;
    if (!(ofp = fopen(output_filename, "a"))) {
	fprintf(stderr, "Can't open output file!\n");
	return -1;
    }
    if (!wrote_header) {
	++wrote_header;
	if (write_crisp_header(ofp) < 0) {
	    fclose(ofp);
	    return -1;
	}
    }

    if (write_release_data(releases, ofp) < 0) {
	fclose(ofp);
	return -1;
    }
    (void) fclose(ofp);
    return 0;
}

/*
 * int write_crisp_header(FILE *ofp) - 
 */
int write_crisp_header(ofp)
    FILE *ofp;
{
    extern struct passwd *getpwuid();
    extern uid_t getuid();
    extern time_t time();

    struct passwd *pwd;
    time_t current_time;
    char   *current_time_string;
    char hostname_buffer[256];

    pwd = getpwuid(getuid());
    time(&current_time);
    gethostname(hostname_buffer, sizeof(hostname_buffer));

    WRITE_SEPARATOR_LINE(ofp);
    fprintf(ofp, "%c CRiSP Calibration data file, version %d.%d.%d\n",
	    COMMENT_CHAR, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    if (putc(COMMENT_CHAR, ofp) == EOF)
	return -1;
    if (putc('\n', ofp) == EOF)
	return -1;

    current_time_string = ctime(&current_time);

    fprintf(ofp, "%c Written by user %s (%s) on %s", COMMENT_CHAR,
	    (pwd && pwd->pw_name) ? pwd->pw_name   : "<Unknown>",
	    (pwd && pwd->pw_gecos) ? pwd->pw_gecos : "<Unknown>",
	    (current_time_string) ? current_time_string : "<Unknown>");
    fprintf(ofp, "%c\ton host %s\n", COMMENT_CHAR, hostname_buffer);

    if (putc(COMMENT_CHAR, ofp) == EOF)
	return -1;
    if (putc('\n', ofp) == EOF)
	return -1;
    WRITE_SEPARATOR_LINE(ofp);
    return 0;
}

int write_release_data(release, ofp)
    ReleaseInfo* release;
    FILE		*ofp;
{
    extern int fputs();
    int i = -1;
    char buf[BUFSIZ];
    
    static int wrote_header=0;

    /* write data header tokens */
    if (!wrote_header) {
	wrote_header = 1;
	fprintf(ofp, "%s %d\n", VERSION_TOKEN, VERSION_VALUE);
    }

    while (release) {
	/* loop through the rls_dates and make a release for each */
	i = -1;
	while (release->rls_dates[++i]) {
	    fputs(RELEASE_TOKEN,	ofp);
	    fputs(release->species,	ofp);
	    putc(' ', ofp);
	    fputs(release->site,	ofp);
	    fprintf(ofp, " %d\n", release->rls_dates[i]);
	    fprintf(ofp, "\t\t%s %s\n", STOCK_TOKEN, release->stock);
	    sprintf(buf, "%s_%d", release->stock, release->rls_dates[i]);
	    fprintf(ofp, "\t\t%s %s\n", RT_STOCK_TOKEN, buf);
	    fprintf(ofp, "\t\t%s %d\n", SMOLT_ONSET_TOKEN, release->smolt_onset);
	    fprintf(ofp, "\t\t%s %d\n", SMOLT_FINISH_TOKEN, release->smolt_finish);
	    fprintf(ofp, "\t\t%s %.2f\n", LENGTH_TOKEN, release->rls_lengths[i]);
	    fputs(QUANTITY_TOKEN,	ofp);
	    fprintf(ofp, "\t%f\n", release->rls_numbers[i]);
	    fputs(END_RELEASE_TOKEN,	ofp);
	    putc('(', ofp);
	    fputs(release->species,	ofp);
	    putc(')', ofp);
	    putc('\n', ofp);
	}
	release = release->next;
    }
    
    return 0;
}


int write_calib_dat(CrispCalib* calib)
{
    char cmd[BUFSIZ];
    char cmd2[BUFSIZ];    
    int erc=0;

    cmd[0] = 0;
    
    unlink(AWK_PROGRAM);
    unlink(CRISP_DATFILE);

    if (calib->awk_script) {
	if (calib->awk_sed_cmd) {
	    sprintf(cmd, "sed %s %s > %s",
		    calib->awk_sed_cmd,
		    calib->awk_script,
		    AWK_PROGRAM);
	}
	else
	    sprintf(cmd, "cp %s %s", calib->awk_script, AWK_PROGRAM);

	sprintf(cmd2, ";awk -f %s %s %s > %s",
		AWK_PROGRAM,
		calib->awk_parameters,
		calib->datafile,
		CRISP_DATFILE);
	strcat(cmd, cmd2);
    }
    else
	sprintf(cmd, "cp %s %s", calib->datafile, CRISP_DATFILE);

    /* append the include directive for the release file */
    sprintf(cmd2, "; echo \"include %s\" >> %s", CRISP_RLSFILE, CRISP_DATFILE);
    strcat(cmd, cmd2);
    
#ifdef DEBUG    
    printf("%s\n", cmd);
#endif    
    return(system(cmd));
}

int run_crisp(CrispCalib* calib)
{
    char cmd[BUFSIZ];
    char flags[8];

    if (tt_flag == 1) 
	sprintf(flags, "-bsit");
    else
	sprintf(flags, "-bsi");
    

    if (calib->river_description)
	sprintf(cmd, "crisp1 %s -r %s -d %s", flags, calib->river_description, CRISP_DATFILE);
    else
	sprintf(cmd, "crisp1 %s -d %s", flags, CRISP_DATFILE);

    return(system(cmd));
}

void AnnounceFatalError() {
	fprintf(stderr, "crisp_calib encountered a fatal error\n");
}
