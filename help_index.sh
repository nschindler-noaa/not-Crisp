#!/usr/local/gnu/bin/bash
#!/bin/sh

mkdir -p help/output
rm -f help/output/help_include.h
echo "char* help_index[] = {" > help/output/help_index_array.h
echo "char* help_index_names[] = {" > help/output/help_index_names_array.h
echo "int help_index_lengths[] = {" > help/output/help_index_lengths_array.h
for i in help/*; do
	if [ -f "$i" ]; then
		tmp="${i//./_}"
		data='help_'"${tmp##*/}"'_data'
		file='help_'"${i##*/}"'_data'
	if [ ! -f "help/output/$file.c" -o "$i" -nt "help/output/$file.c" ]; then
		./bin2c "$i" "$data" "help/output/$file.h" > "help/output/$file.c"
	fi
		echo "	&$data[0]," >> help/output/help_index_array.h
		echo "	\"${i#help/}\"," >> help/output/help_index_names_array.h
		echo "	$data"'_length,' >> help/output/help_index_lengths_array.h
		echo "#include \"$file.h\"" >> help/output/help_include.h
	fi
done
echo "NULL };" >> help/output/help_index_array.h
echo "NULL };" >> help/output/help_index_names_array.h
echo "-1 };" >> help/output/help_index_lengths_array.h
echo "#include \"help_index_array.h\"" >> help/output/help_include.h
echo "#include \"help_index_names_array.h\"" >> help/output/help_include.h
echo "#include \"help_index_lengths_array.h\"" >> help/output/help_include.h

