all: run

BAZEL=$(shell which bazel)

.PHONY: run
run: build
	$(BAZEL) run //cpp:main

.PHONY: build
build: 
	$(BAZEL) build -c dbg //cpp:main --copt='-std=c++17' --copt='-g' --strip=never --spawn_strategy=standalone

.PHONY: cleam
clean: 
	$(BAZEL) clean