.\crf_learn template train.data model >> model_out.txt
.\crf_test -m model test.data >> output.txt
Get-Content output.txt | perl conlleval.pl -d "\t" > perl_out.txt
