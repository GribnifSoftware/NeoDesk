XFONTINFO fontinfo[9] = {
    { 1, 9, 5, 5 },
    { 1, 9, 17, 5 },
    { 1, 10, 11, 3 },
    { 1, 10, 11, 3 },
    { 1, 10, 10, 3 },
    { 1, 10, 11, 3 },
    { 1, 10, 11, 3 },
    { 1, 10, 3, 3 },
    { 1, 10, 11, 3 } };

APPFLAGS dflt_flags =  { "", "", { 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1,  0,  0 } };

OBJECT ascii_tbl[] = {
{ -1, 1,  6, G_BOX,    NONE, NORMAL, 0xFF1170L, 0, 0, 51, 5 },
{ 2, -1, -1, G_STRING, TOUCHEXIT, NORMAL, (long)"\x01\x02\x03\x04\x05\x06\x07\x08\
\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\
\x1C\x1D\x1E\x1F\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\
\x2F\x30\x31\x32\x33", 0, 0, 51, 1 },
{ 3, -1, -1, G_STRING, TOUCHEXIT, NORMAL, (long)"456789:;<=>?@ABCDEFGHIJKLMNOPQRS\
TUVWXYZ[\\]^_\`abcdef", 0, 1, 51, 1 },
{ 4, -1, -1, G_STRING, TOUCHEXIT, NORMAL, (long)"ghijklmnopqrstuvwxyz{|}~€‚ƒ„…†\
‡‰‹‘’“”•–—™", 0, 2, 51, 1 },
{ 5, -1, -1, G_STRING, TOUCHEXIT, NORMAL, (long)"› ΅Ά£¤¥¦§¨©ª«¬­®―°±²³΄µ¶·ΈΉ\
Ί»Ό½ΎΏΐΑΒΓΔΕΖΗΘΙΚΛΜ", 0, 3, 51, 1 },
{ 6, -1, -1, G_STRING, TOUCHEXIT, NORMAL, (long)"ΝΞΟΠΡÒΣΤΥΦΧΨΩΪΫάέήίΰαβγδεζηθικλμ\
νξοπρςστυφχψωϊϋόύώÿ", 0, 4, 51, 1 },
{ 0, -1, -1, G_IBOX, HIDETREE, NORMAL, 0L, 0, 0, 1, 1 } };
