from ROOT import TColor

palette = {
	'blue':		TColor(TColor.GetFreeColorIndex(),	31./255.,	119./255.,	180./255.	),
	'orange':	TColor(TColor.GetFreeColorIndex(),	255./255.,	127./255.,	14./255.	),
	'green':	TColor(TColor.GetFreeColorIndex(),	44./255.,	160./255.,	44./255.	),
	'red':		TColor(TColor.GetFreeColorIndex(),      214./255.,	39./255.,	40./255.	),
	'purple':	TColor(TColor.GetFreeColorIndex(),      148./255.,	103./255.,	189./255.	),
	'brown':	TColor(TColor.GetFreeColorIndex(),      140./255.,	86./255.,	75./255.	),
	'pink':		TColor(TColor.GetFreeColorIndex(),      227./255.,	119./255.,	194./255.	),
	'grey':		TColor(TColor.GetFreeColorIndex(),      127./255.,	127./255.,	127./255.	),
	'lightgreen':	TColor(TColor.GetFreeColorIndex(),      188./255.,	189./255.,	34./255.	),
	'cyan':		TColor(TColor.GetFreeColorIndex(),      23./255.,	190./255.,	207./255.	)
}

palette['neutral'] = palette['grey']
palette['e-'] = palette['blue']
palette['pi-'] = palette['red']
palette['mu-'] = palette['orange']
palette['gamma'] = palette['green']
palette['Scnt'] = palette['red']
palette['Ckov'] = palette['blue']
palette['Chi'] = palette['purple']
palette['Energy'] = palette['green']