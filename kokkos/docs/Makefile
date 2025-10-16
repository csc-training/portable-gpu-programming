FACTORY=slidefactory.sif
OPTIONS=slides

SRC=$(wildcard ??-*.md)
HTML=$(patsubst %.md,%.html,$(SRC))
PDF=$(patsubst %.md,%.pdf,$(SRC))

.PHONY: html pdf clean

html: $(HTML)

pdf: $(PDF)

clean:
	-rm -f $(HTML) $(PDF)

%.html: %.md
	$(FACTORY) $(OPTIONS) --format html $<

%.pdf: %.md
	$(FACTORY) $(OPTIONS) --format pdf $<
