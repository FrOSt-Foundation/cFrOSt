void puts(char* s) {
    u16* p = (u16 *) 0xF001;
    *p = (u16) s;
    interrupt(0x0000);
}
