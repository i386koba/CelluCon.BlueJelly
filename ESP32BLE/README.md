# ESP-WROOM-32
ESP32最小構成ボード [K-ESP32T]　http://www.aitendo.com/product/15562

回路図？　http://www.shenzhen2u.com/image/catalog/Module/NodeMCU-32S/nodemcu_32s_sch.png

ESP32 Dev Module を選び、ハードウェアを https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf の通りに配線すると、書き込み時のモード変更とリセットを勝手にやってくれます。

配線は全部必要というわけではなく、シリアルのDTS,RTS と ESP32のEN,IO0 の間だけ回路図通りならOKです。

いろいろな ESP-WROOM-32

https://macsbug.wordpress.com/category/esp32/

## 開発視点の超簡単BLE入門
http://jellyware.jp/kurage/bluejelly/ble_guide.html

- GAP（Generic Access Profile）
    - セントラル　(Web)　→　ペリフェアラル（ESP32)
    - Advertise ペリフェラルが「私を見つけて！」と電波を発する行為
    - Scan　セントラルが「周りにどれだけペリフェラル居るかな？」と周辺のペリフェラルを探す行為
    - Connect　セントラルが「こいつに決めた！」と特定のペリフェラルと接続する処理。
        - ユーザーに選択させます
    - DiSconnect　接続解除。セントラルがアプリを終了する前などに実行します。
        - DisconnectせずにScanすると見つからない場合があり。

- GATT（Generic Attributes汎用アトリビュート）
    - プロファイル　ペリフェアラル（ESP32)内に 
        - Service (フォルダのようなもの)　
           - SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
        - Characteristic（ファイルのようなもの)
            - 各Serviceの中に１つ以上のCharacteristicがあります
            - CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
            - CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
        - Characteristicの中身には
            - Value　データそのものです。まさにこのデータをBLEでやりとりします。
            - Property　read, write, notify のどれに対応していのかを示す属性です。
                - Read　はセントラルがペリフェラルからデータを読むコマンド
                - Write　はセントラルがペリフェラルに対してデータを書き込むコマンド
                - Notify　ペリフェラル側のタイミングで継続的にセントラルへデータ送信させるコマンド
            - Descriptor　追加情報のこと
        
### 基本的には
- セントラル（Web)からペリフェラル（ESP32)へはコマンド20バイト内の書き込み
    - Webからは Write
    - ESP32 で　`void onWrite(BLECharacteristic *pCharacteristic) {rxValue = pCharacteristic->getValue();}` で受信
- ペリフェラル（ESP32)からセントラル（Web)へは大きなデータ（GPSとか）を渡したい
    - Webからは Read、Notify 
    - ESP32 のReadされるデータの配置は　`pCharacteristic->setValue(&txValue, 1);`

- https://github.com/nkolban/ESP32_BLE_Arduino
- BLE2902.h
    - Notify 終了の Descriptor（追加情報）
    - `pCharacteristic->addDescriptor(new BLE2902());`

    BLE C++ Guide
        https://github.com/nkolban/esp32-snippets/blob/master/Documentation/BLE%20C%2B%2B%20Guide.pdf 
    より

    Associated with the idea of indications/notifications, is the architected BLE Descriptor called ["Client Characteristic Configuration"](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml) which has UUID 0x2902. 
       
    This contains (among other things) two distinctbit fields that can be on or off.  
    
    One bit field governs Notifications while the other governs Indications. 
    
    If the corresponding bit is on, then the server can/may send the corresponding push.  
    
    For example if theNotifications bit is on, then the server can/may send notifications.  
    
    The primary purpose of the descriptor is to allow a partner (a client) to request that the server actually send notifications or indications.  
    
    Here is an example.
    
    Imagine that we have a BLE Server that can publish notifications when data changes.  
    
    Now imagine that a BLE Client connects but is actually not interested in receiving these.  
    
    If the BLE Server executes a notification push and sends the data, that will be wasted effort/energy as the client doesn't want or can't use the information. 
    
    What we really want is that the client should inform the server that if it wantsto push new data then it either should or shouldn't.  
    
    And that is where this descriptor comes into play.  
    
    If the descriptor is present on a characteristic then then client can remotely change the bit flag to enableor disable notifications and indications.  
    
    Since the descriptor flag is stored locally within the server, the server is at liberty to examine the flag before performing a radio transmission.
    
    What this means is that the client/peer can toggle on or off its desire to receive notifications and the server should honor those requests.
    
    The BLE specification constrains the maximum amount of data that can be sent through a notification or indication to be 20 bytes or less.  
    
    Take this into account in your designs.  
    
    If the value of a characteristic is greater than this amount, then only the first 20 bytes of the data will be transmitted.It is likely that your own BLE server application is going to expose its own set of characteristics.  
    
    Through a characteristic you can set and get the value as a binary piece of storage, however this may betoo low level for you.  
    
    An alternative is to utilize the features of the C++ language and model your ownspecialized characteristic as a sub class of BLECharacteristic.  
    
    This could then encapsulate the lowerlevel value's getter and setter with your own customized version.For example, if your characteristic represented a temperature, you could create:
    
        class MyTemperatureCharacteristic: public BLECharacteristic {
            MyTemperature(): BleCharacteristic(BLEUUID(MYUUID)) {
                setTemperature(0.0);
            }
            void setTemperature(double temp) {
                setValue(&temp, sizeof temp);
            }
            double getTemperature() {
                return *(double*)getValue();
            }
        }





