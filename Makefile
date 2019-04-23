all: run

BAZEL=$(shell which bazel)

.PHONY: run
run: build
	$(BAZEL) run //cpp:main

.PHONY: build
build: 
	$(BAZEL) build //cpp:main

.PHONY: cleam
clean: 
	$(BAZEL) clean