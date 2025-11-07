// tests/integration/src/tags.h:49:0
// foo
int  a1 has foo tag


// bar


// tags {
a1 foo=

// }
// foo


// bar
int  a2 has bar tag 


// tags {
a2 bar=

// }
// foo


// bar
int  a3 has bar tag (value=a3) 


// tags {
a3 bar=a3
a3 bar=a4

// }
// foo


// bar
int  a4 has bar tag (value="a3", a4, foo) 


// tags {
a4 bar="a3", a4, foo
a4 bar='a5', a6, bar

// }


