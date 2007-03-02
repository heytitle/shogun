import shogun.Features as sf
f=sf.StringCharFeatures(sf.Alphabet(sf.RAWBYTE))
print f.load_from_directory(".")
print f.get_max_vector_length()
print f.get_num_vectors()
print f.get_vector_length(0)
print f.get_feature(0,0)
print f.get_feature(0,1)
print f.get_feature(0,2)
f.select_feature_vector(0)
v=f.get_str()
print v

f2=sf.StringCharFeatures(sf.Alphabet(sf.DNA))
f2.set_string_features(['hey','guys','i','am','a','string'])

f2.select_feature_vector(1)
v=f2.get_str()
print v
