
#!/bin/sh

NOW=$(date+"%Y-%m-%d_%H-%M-%S")

tag=$1
latex images_latex.tex

latex images_latex.tex

dvips images_latex.dvi

ps2pdf images_latex.ps

#OUT=$(echo "${tag}_latex_plots_${NOW}.pdf" | tr -d ' ')
OUT=${tag}_latex_plots_${NOW}.pdf

echo "timestamp: "$NOW
echo "Creating "$OUT
mv images_latex.pdf $OUT

ls #evince images_latex_${NOW}.pdf &
