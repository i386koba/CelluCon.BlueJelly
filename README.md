# CelluCon.BlueJelly
携帯電話網利用の操縦装置開発のオープンソースプロジェクト　https://github.com/cellucon

をBLEでやろうとして、BlueJellyで実装しようとしているところ。

https://i386koba.github.io/CelluCon.BlueJelly/ からBLEセラコンWeb画面を開けるようにする予定。(まだサンプル画面)

現状　https://qiita.com/b56ff3e7b3ca207ae783/ の記事にある用に
https://i386koba.github.io/CelluCon.BlueJelly/org/read.html
がESP32と動くだけです。

ESP32のコードは
https://github.com/i386koba/CelluCon.BlueJelly/blob/master/ESP32BLE/BLE_server1.ino

## BlueJelly

BlueJellyはWeb Bluetooth APIを簡単に扱うことができるラッパーライブラリです。 HTMLとJavaScriptだけでBLEをプログラミングできます。

https://github.com/electricbaka/bluejelly

BlueJellyって何だよ！

http://jellyware.jp/kurage/bluejelly/getting_started.html

# RN4020

RN4020でI2Cの値を読み取る

http://masato-ka.hatenablog.com/entry/2017/10/17/084148

## ESP-WROOM-32
ESP32最小構成ボード [K-ESP32T]　http://www.aitendo.com/product/15562

回路図？　http://www.shenzhen2u.com/image/catalog/Module/NodeMCU-32S/nodemcu_32s_sch.png

ESP32 Dev Module を選び、ハードウェアを https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf の通りに配線すると、書き込み時のモード変更とリセットを勝手にやってくれます。

配線は全部必要というわけではなく、シリアルのDTS,RTS と ESP32のEN,IO0 の間だけ回路図通りならOKです。

いろいろな ESP-WROOM-32

https://macsbug.wordpress.com/category/esp32/


http://jellyware.jp/kurage/bluejelly/ble_guide.html

-GAP（Generic Access Profile）
    -セントラル　(Web)　→　ペリフェアラル（ESP32)
    -Advertise ペリフェラルが「私を見つけて！」と電波を発する行為
    -Scan　セントラルが「周りにどれだけペリフェラル居るかな？」と周辺のペリフェラルを探す行為
    -Connect　セントラルが「こいつに決めた！」と特定のペリフェラルと接続する処理。ユーザーに選択させます
    -DiSconnect　接続解除。セントラルがアプリを終了する前などに実行します。DisconnectせずにScanすると見つからない場合があ

-GATT（Generic Attributes汎用アトリビュート）
    -プロファイル　ペリフェアラル（ESP32)内に 
        -Service (フォルダのようなもの)　
           -SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
        -Characteristic（ファイルのようなもの)
            -各Serviceの中に１つ以上のCharacteristicがあります
            -CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
            -CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
        -Characteristicの中身には
            -Value　データそのものです。まさにこのデータをBLEでやりとりします。
            -Property　read, write, notify のどれに対応していのかを示す属性です。
                -Read　はセントラルがペリフェラルからデータを読むコマンド
                -Write　はセントラルがペリフェラルに対してデータを書き込むコマンド
                -Notify　ペリフェラル側のタイミングで継続的にセントラルへデータ送信させるコマンド
            -Descriptor　追加情報のこと
        


基本的には
・セントラル（Web)からペリフェラル（ESP32)へはコマンド20バイト内の書き込み　
（Write　で用が足ります)
・ペリフェラル（ESP32)からセントラル（Web)へは大きなデータ（GPSとか）を渡したい
（Read、Notify）





