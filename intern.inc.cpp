struct symt_entry;
struct symt_entry {
	const char *sym;
	int val;
	struct symt_entry *next;
};

long symt_next = 1;
struct symt_entry *symt_root = NULL;

const int symt_align = 8;
const int symt_chunk_sz = 512;
char *symt_chunk = 0;
int symt_chunk_pos = 0;

char *intern_alloc_chunk() {
	char *chunk = (char*)malloc(symt_chunk_sz);
	if (!chunk) {
		fprintf(stderr, "failed to allocated intern chunk\n");
		exit(1);
	}
	return chunk;
}

char *intern_get_slice(int len) {
	if (len > symt_chunk_sz) {
		char *slice = (char*)malloc(len);
		if (!slice) {
			fprintf(stderr, "failed to allocated intern slice");
			exit(1);
		}
		return slice;
	}
	if (symt_chunk_pos + len > symt_chunk_sz) {
		symt_chunk = intern_alloc_chunk();
		symt_chunk_pos = 0;
	}
	len = (len + symt_align - 1) & ~(symt_align - 1);
	char *slice = symt_chunk + symt_chunk_pos;
	symt_chunk_pos += len;
	return slice;
}

char *intern_alloc_string(const char *str, int len) {
	char *slice = intern_get_slice(len + 1);
	for (int i = 0; i < len; ++i) {
		slice[i] = str[i];
	}
	slice[len] = 0;
	return slice;
}

void mc_intern_init() {
	symt_chunk = intern_alloc_chunk();
}

int mc_intern(const char *str, int len) {
	struct symt_entry *curr = symt_root;
	while (curr) {
		if (streql(curr->sym, str, len)) {
			return curr->val;
		}
		curr = curr->next;
	}
	struct symt_entry *item = (struct symt_entry*)malloc(sizeof(struct symt_entry));
	item->sym = intern_alloc_string(str, len);
	item->val = symt_next++;
	item->next = symt_root;
	symt_root = item;
	return item->val;
}