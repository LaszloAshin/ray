#include <mach-o/loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // chmod

const char* name_load_command(uint32_t cmd) {
	switch (cmd) {
	case LC_SYMTAB: return "LC_SYMTAB";
	case LC_UNIXTHREAD: return "LC_UNIXTHREAD";
	case LC_SEGMENT_64: return "LC_SEGMENT_64";
	case LC_SOURCE_VERSION: return "LC_SOURCE_VERSION";
	}
	return "UNKNOWN";
}

struct thread_command_unixthread64 {
	uint32_t cmd;
	uint32_t cmdsize;
	uint32_t flavor;
	uint32_t count;
	x86_thread_state64_t state;
};

int is_payload_eligible(const struct segment_command_64* seg, const struct section_64* sect) {
	return !strcmp(seg->segname, "__TEXT") || (!strcmp(seg->segname, "__DATA") && !strcmp(sect->sectname, "__data"));
}

int main(int argc, char* argv[]) {
	(void)argc;
	int v = !strcmp(argv[1], "-v");
	v && ++argv;
	FILE* fp = fopen(argv[1], "rb");

	struct mach_header_64 hd;
	fread(&hd, sizeof(struct mach_header_64), 1, fp);
	v && printf("number of load commands: %d\n", hd.ncmds);
	v && printf("size of load commands: %d\n", hd.sizeofcmds);

	char* p = (char*)malloc(hd.sizeofcmds);
	fread(p, hd.sizeofcmds, 1, fp);
	char* q = p;
	uint32_t payload_file_offset = 0;
	uint64_t payload_file_size = 0;
	uint64_t payload_addr = 0;
	uint64_t payload_entry_point = 0;
	int eligible_section_index = 0;

	for (unsigned i = 0; i < hd.ncmds; ++i) {
		struct load_command* lc = (struct load_command*)q;
		v && printf("- load_command: %s(%d)\n", name_load_command(lc->cmd), lc->cmd);
		v && printf("  size: %d\n", lc->cmdsize);

		if (lc->cmd == LC_SEGMENT_64) {
			struct segment_command_64* seg = (struct segment_command_64*)lc;
			v && printf("  segment name: %s\n", seg->segname);
			v && printf("  vmaddr: 0x%llx\n", seg->vmaddr);
			v && printf("  vmsize: 0x%llx\n", seg->vmsize);
			v && printf("  fileoff: 0x%llx\n", seg->fileoff);
			v && printf("  filesize: 0x%llx\n", seg->filesize);
			v && printf("  number of sections: %d\n", seg->nsects);

			struct section_64* sect = (struct section_64*)(q + sizeof(struct segment_command_64));
			for (unsigned j = seg->nsects; j; --j, ++sect) {
				v && printf("  - section name: %s\n", sect->sectname);
				v && printf("    addr: 0x%llx\n", sect->addr);
				v && printf("    size: 0x%llx\n", sect->size);
				v && printf("    offset: 0x%x\n", sect->offset);

				if (is_payload_eligible(seg, sect)) {
					if (!eligible_section_index) {
						payload_addr = sect->addr;
						payload_file_offset = sect->offset;
					}
					payload_file_size = sect->offset + sect->size - payload_file_offset;
					++eligible_section_index;
				}
			}
		} else if (lc->cmd == LC_UNIXTHREAD) {
			struct thread_command_unixthread64* tc = (struct thread_command_unixthread64*)lc;
			v && printf("  rip: 0x%llx\n", tc->state.__rip);
			payload_entry_point = tc->state.__rip;
		}
		q += lc->cmdsize;
	}

	free(p);

	v && printf("payload_file_offset = 0x%x\n", payload_file_offset);
	v && printf("payload_file_size = 0x%llx\n", payload_file_size);

	p = (char*)malloc(payload_file_size);
	fseek(fp, payload_file_offset, SEEK_SET);
	fread(p, payload_file_size, 1, fp);
	fclose(fp);

	char payload_filename[64];
	snprintf(payload_filename, 64, "%s.payload", argv[1]);

	fp = fopen(payload_filename, "wb");
	fwrite(p, payload_file_size, 1, fp);
	fclose(fp);

	free(p);

	char paq_filename[64];
	snprintf(paq_filename, 64, "%s.paq", argv[1]);

	const int paq_mode = 3;
	const int paq_complexity = 1;

	char onekpaq_command_line[4096];
	snprintf(onekpaq_command_line, 4096, "./onekpaq %d %d %s %s", paq_mode, paq_complexity, payload_filename, paq_filename);
	v && puts(onekpaq_command_line);

	fp = popen(onekpaq_command_line, "r");
	if (!fp) {
		fprintf(stderr, "onekpaq failed\n");
		return !0;
	}

	int paq_offset = -1;
	int paq_shift = -1;
	fscanf(fp, "P offset=%d shift=%d\n", &paq_offset, &paq_shift);
	fclose(fp);

	fp = fopen(paq_filename, "rb");
	int paq_size = -1;
	for (int i = -1;; --i) {
		fseek(fp, i, SEEK_END);
		paq_size = ftell(fp) + 1;
		char ch;
		fread(&ch, 1, 1, fp);
		if (ch) break;
	}

	char output_filename[64];
	snprintf(output_filename, 64, "%s.compact", argv[1]);

	char nasm_command_line[4096];
	char* const end = nasm_command_line + 4096;
	p = nasm_command_line;

	p += snprintf(p, end - p, "nasm");
	p += snprintf(p, end - p, " -DPAYLOAD_ADDR=0x%llx", payload_addr);
	p += snprintf(p, end - p, " -DPAYLOAD_SIZE=0x%llx", payload_file_size);
	p += snprintf(p, end - p, " -DPAYLOAD_ENTRY_POINT=0x%llx", payload_entry_point);
	p += snprintf(p, end - p, " -DPAYLOAD_FILENAME='\"%s\"'", payload_filename); // XXX this needs to go
	p += snprintf(p, end - p, " -DPAQ_FILENAME='\"%s\"'", paq_filename);
	p += snprintf(p, end - p, " -DPAQ_OFFSET=%d", paq_offset);
	p += snprintf(p, end - p, " -DPAQ_SIZE=%d", paq_size);
	p += snprintf(p, end - p, " -DONEKPAQ_DECOMPRESSOR_MODE=%d", paq_mode);
	p += snprintf(p, end - p, " -DONEKPAQ_DECOMPRESSOR_SHIFT=%d", paq_shift);
	p += snprintf(p, end - p, " -o%s", output_filename);
	p += snprintf(p, end - p, " ../src/compact-macho.s");
	v && puts(nasm_command_line);

	int ret = system(nasm_command_line);
	if (ret) return ret;

	v && printf("chmod 0755 %s\n", output_filename);
	chmod(output_filename, 0755);
}
