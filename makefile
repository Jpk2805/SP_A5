SUBDIRS := DP-1 DP-2 DC

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for d in $(SUBDIRS); do \
	  $(MAKE) -C $$d clean; \
	done

