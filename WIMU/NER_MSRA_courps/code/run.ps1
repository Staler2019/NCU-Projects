# install with conda

# conda activate wimuta_hw3_ner_env

pip install -r .\requirements.txt
# conda install -c anaconda ipython

### end of installation of env


# run training & prediction: edit parm in .\code\system.config
python .\main.py

# print accuracy compute by perl
Get-Content .\data\example_datasets_msra\label_test_30.txt | perl ..\CRF++-0.58\example\chunking\conlleval.pl -d " " > perl_out.txt