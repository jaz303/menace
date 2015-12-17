int streql(const char *str1, const char *str2, int len2) {
	int p = 0;
    while (str1[p] && p < len2) {
        if (str1[p] != str2[p]) return 0;
        p++;
    }
    return str1[p] == 0 && p == len2;
}