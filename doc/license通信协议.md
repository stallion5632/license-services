#   变更
# 消息结构

| Message     | Length                                       | Description         | Note                                      |
| ----------- | -------------------------------------------- | ------------------- | ----------------------------------------- |
| MessageHead | 4 (unsigned int)                             | Sync Bytes          | 0x02, 0x00, 0x00, 0x00                    |
| MessageHead | 4 (unsigned int)                             | Full Message Length | Length of MessageHead+JsonBody+BinaryData |
| MessageHead | 4 (unsigned int)                             | Message Seq         | Increasing message sequential number      |
| MessageHead | 4 (unsigned int)                             | Message Type        | eg: login_req                             |
| MessageHead | 4(unsigned int)                              | Json body length    | Length of json string                     |
| MessageHead | 12(unsigned char)                            | Reserved            |                                           |
| Body        | json length                                  | Json message        |                                           |
| Body        | FullMessageLength - MessageHead - JsonLength | BinaryData          | Image or trajectory data                  |

注：消息头固定为32字节

# 消息体结构

## 错误码定义

| 响应码 | 描述           |
| ------ | -------------- |
| 1000   | 响应成功       |
| -1     | 响应失败       |
| 1001   | 未知错误       |
| 1002   | 未登录         |
| 1003   | 消息体解析失败 |
| 1005   | 消息过载       |
| 1006   | 参数错误       |

## message_types

下面为和服务端交互的所有消息号

```json
{
    "sync_bytes": "0x09,0x00,0x00,0x00",
    "message_types": {
        "heart_req": "0x09,0x00,0x01,0x01",
        "heart_rsp": "0x09,0x00,0x01,0x02",
        "login_req": "0x09,0x00,0x02,0x01",
        "login_rsp": "0x09,0x00,0x02,0x02",
        "logout_req": "0x09,0x00,0x03,0x01",
        "logout_rsp": "0x09,0x00,0x03,0x02",
        "get_dogstatus_req": "0x09,0x00,0x04,0x01",
        "get_dogstatus_rsp": "0x09,0x00,0x04,0x02",
        "dog_status_ntf": "0x09,0x00,0x05,0x03"
    },
    "remarks": "this is client for dog test"
}
```

##  LoginReq（认证）

###  login_req（认证请求）

| Name             | login_req            |
| ---------------- | ----------------------------- |
| **Message Type** | 0x09,0x00,0x01,0x01 |
| **Direction**    | **Client -> Server**          |

示例：

```json
{
    "msg_type": "login_req",
    "version" : "1.0", 
    "timestamp" : 1528686091000, 
    "data" : {
    	"ip" : "127.0.0.1",
    	"mac" : ""
	}
}
```

注解：

| 字段名 | 描述    | 备注 |
| ------ | ------- | ---- |
| ip     | ip地址  |      |
| mac    | mac地址 | 可选 |

### login_rsp

| Name             | login_rsp            |
| ---------------- | ----------------------------- |
| **Message Type** | 0x09,0x00,0x01,0x02 |
| **Direction**    | **Server -> Client**          |

示例：

```json
{
    "msg_type": "login_rsp",
    "version" : "1.0", 
    "timestamp" : 1528686091000, 
    "data" : {
    	"code" : "1000",
        "dog_status" : "0",
    	"message" : "english only", 
	}
}
```

注解：

| 字段名     | 说明                   | 备注 |
| ---------- | ---------------------- | ---- |
| code       | 操作操作码，1000为正常 |      |
| dog_status | usb狗状态              |      |
| message    | 提示消息，必须为英文   | 可选 |
##  Logout
### logout_req（释放license）

| Name             | logout_req           |
| ---------------- | ----------------------------- |
| **Message Type** | 0x09,0x00,0x02,0x01 |
| **Direction**    | **Client -> Server**          |

示例：

```json
{
    "msg_type": "logout_req",
    "version" : "1.0", 
    "timestamp" : 1528686091000, 
    "data" : {
    	"ip" : "127.0.0.1",
    	"mac" : ""
	}
}
```

注解：

| 字段名 | 描述    | 备注 |
| ------ | ------- | ---- |
| ip     | ip地址  |      |
| mac    | mac地址 | 可选 |

### logout_rsp

| Name             | logout_rsp           |
| ---------------- | ----------------------------- |
| **Message Type** | 0x09,0x00,0x02,0x02 |
| **Direction**    | **Server -> Client**          |

示例：

```json
{
    "msg_type": "logout_rsp",
    "version" : "1.0", 
    "timestamp" : 1528686091000, 
    "data" : {
    	"code" : "1000",
        "dog_status" : "0",
    	"message" : "English Only"
	}
}
```

注解：

| 字段名     | 说明                   | 备注 |
| ---------- | ---------------------- | ---- |
| code       | 操作操作码，1000为正常 |      |
| dog_status | usb狗状态              |      |
| message    | 提示消息，必须为英文   | 可选 |

## dog_status_ntf

| Name            | dog_status_ntf       |
| --------------- | -------------------- |
| **Meesag Type** | 0x09,0x00,0x04,0x03  |
| **Direction**   | **Server -> Client** |

```json
{
    "msg_type": "dog_status_ntf",
    "version" : "1.0" , 
    "timestamp" : 1528686091000, 
    "data" : {
        "dog_status" : "0",
        "message" : "English Only"
	}
}
```

注解：

| 字段名     | 说明                 | 备注 |
| ---------- | -------------------- | ---- |
| dog_status | usb狗状态            |      |
| message    | 提示消息，必须为英文 | 可选 |