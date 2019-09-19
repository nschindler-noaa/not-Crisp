/* Define GENERATE_ASM to generate output for GNU assmbler,
 * otherwise C code will be output */

#include <stdio.h>
#include <string.h>

#define COLUMNS	16
#ifdef GENERATE_ASM
#define	OUTPUT_FORMAT	"assembly"
#else
#define	OUTPUT_FORMAT	"C code"
#endif

int main(int argc, const char** argv) {
	if (argc < 3 || argc > 4) {
		fprintf(stderr, "USAGE: %s FILE VARIABLE [HEADER]\n", argv[0]);
		fprintf(stderr, "FILE will be converted from binary to "OUTPUT_FORMAT" for VARIABLE on standard output\n");
		fprintf(stderr, "If specified, then a C header file will be written to HEADER\n");
	}
	else {
		FILE *f = fopen(argv[1], "rb");
		FILE *h;
		
		if (argc > 3) h = fopen(argv[3], "w");

		if (!f) {
			fprintf(stderr, "ERROR: could not open file(s)\n");
			return 1;
		}
		else {
			int length, byte;
			int column = 0;
	
			fseek(f, 0, SEEK_END);
			length = ftell(f);
			rewind(f);

#ifdef GENERATE_ASM
			printf(".globl %s\n", argv[2]);
			printf("\t.data\n");
			printf("\t.type %s, @object\n", argv[2]);
			printf("\t.size %s, %d\n", argv[2], length);
			printf("%s:\n", argv[2]);
#else
			printf("char %s[%d] = {\n", argv[2], length);
#endif

			if (h) {
				fprintf(h, "#ifndef __embed_%s_length_h\n", argv[2]);
				fprintf(h, "#define __embed_%s_length_h\n", argv[2]);
				fprintf(h, "#define %s_length %d\n", argv[2], length);
				fprintf(h, "extern char %s[%s_length];\n", argv[2], argv[2]);
				fprintf(h, "#endif\n");
				fclose(h);
			}

			while ((byte = fgetc(f)) != EOF) {
#ifdef GENERATE_ASM
				if (column == 0) printf("\t.byte ");
				else printf(",");
#else
				if (column == 0) printf("\t");
#endif

#ifdef GENERATE_ASM
				printf("%3u", byte);
#else
				printf("%3d,", (char)byte);
#endif
				column = (column + 1) % COLUMNS;

				if (column == 0) printf("\n");
			}
#ifdef GENERATE_ASM
			printf("\n");
#else
			printf("\n};\n");
#endif
	
			fclose(f);
		}
	}

	return 0;
}
