# yk2_save
 Tool for encrypting/decrypting Yakauza Kiwami 2 saves.

Original decryption code from [here](https://gist.github.com/simontime/59661a189b20fc3517b20d8c9f329017) by simontime. I added proper checksum calculation and split the tool into two executables for easier usage.

## Usage

`yk2_decrypt.exe data.sav` will output into `data.json`.

`yk2_encrypt.exe data.json` will output into `data.sav`.

Alternatively you can just drag and drop the file onto the respective exe.
