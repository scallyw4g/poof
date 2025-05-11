// tests/integration/src/tags.h:39:0

// has_tag bar (outer members)
(tag_struct b1) has bar tag (value=b1)


// has_tag foo (inner members)
(tag_struct b1)  >  (int  a1) has foo tag



// has_tag bar (inner members)
(tag_struct b1)  >  (int  a2) has bar tag (value=a2)


// has_tag bar (outer members)
(tag_struct b2) has bar tag (value=b2)


// has_tag foo (inner members)
(tag_struct b2)  >  (int  a1) has foo tag



// has_tag bar (inner members)
(tag_struct b2)  >  (int  a2) has bar tag (value=a2)

