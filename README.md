Dbc2MySQL
=========

Usage:
Dbc2MySQL -[var] [value]
	-u set mysql user
	-p set mysql password
	-h set mysql host (OPTIONAL - default: localhost)
	-o set mysql port (OPTIONAL - default: 3306)
	-d set mysql database (OPTIONAL - default: dbc)
	-s (yes/no) dump all dbc to sql files (OPTIONAL - default: no)
	-x set the structure.xml file

Example: Dbc2MySQL -u root -p pass -x StructureWotLK.xml