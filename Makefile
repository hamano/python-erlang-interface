.PHONY: build

all: build

build:
	python setup.py build
	cp build/lib.*/*.so ./

clean:
	rm -rf build/ MANIFEST

sdist:
	python setup.py sdist

upload:
	python setup.py sdist upload
