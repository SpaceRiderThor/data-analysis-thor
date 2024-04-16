with open('resolution.txt', 'r') as f:
	with open('res.txt', 'w') as w:
		a = 5.00
		for i in f:
			energy = i[0]
			w.write("CdTeDetector.EnergyResolution Gauss {} {}".format(a,i))
			a += 1
		
