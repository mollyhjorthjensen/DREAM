from ROOT import TColor

palette = {
	'blue':		TColor(TColor.GetFreeColorIndex(),	31.,	119.,	180.	),
	'orange':	TColor(TColor.GetFreeColorIndex(),	255.,	127.,	14.	),
	'green':	TColor(TColor.GetFreeColorIndex(),	44.,	160.,	44.	),
	'red':		TColor(TColor.GetFreeColorIndex(),      214.,	39.,	40.	),
	'purple':	TColor(TColor.GetFreeColorIndex(),      148.,	103.,	189.	),
	'brown':	TColor(TColor.GetFreeColorIndex(),      140.,	86.,	75.	),
	'pink':		TColor(TColor.GetFreeColorIndex(),      227.,	119.,	194.	),
	'grey':		TColor(TColor.GetFreeColorIndex(),      127.,	127.,	127.	),
	'lightgreen':	TColor(TColor.GetFreeColorIndex(),      188.,	189.,	34.	),
	'cyan':		TColor(TColor.GetFreeColorIndex(),      23.,	190.,	207.	)
}

palette['neutral'] = palette['grey']
palette['e-'] = palette['blue']
palette['pi-'] = palette['red']
