COMMENT [/][/].*\r?\n
NEWLINE  \r?\n
DIGIT    [0-9]
HEXDIGIT	[0-9a-fA-F]
IDENTIFIER	[a-zA-Z]([a-zA-Z0-9_]*[a-zA-Z0-9])?
SPACE		[ \t]
CSTART		[/][*]
CEND		[*][/]
STRINGSTART	["]
STILLSTRINGA	[^"\\]
STILLSTRINGB	[\\][btrn"\\]
STRINGDONE	["]
FSTRINGSTART	["]
FSTILLSTRINGA	[^"\\]
FSTILLSTRINGB	[\\].
FSTRINGDONE	["]
UNITTYPEINT	['][a-zA-Z0-9][a-zA-Z0-9]?[a-zA-Z0-9]?[a-zA-Z0-9]?[']

%%
FunctionDeclaration : function  ;

rules
%%
programs