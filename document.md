*注意：该项目要求提供完整的《开发文档》（包括模块划分图、每个模块的功能、类设计、文件设计等）以及完整的《使用手册》（包括系统安装手册、用户手册等），其完成情况记入易用度评分。*

# 开发文档

## 类设计

### User
保存用户的基本信息(name, password, email, phone, id, privilege)和登录状态。

###Station
保存站点的基本信息(name, arriveTime, startTime, stopover, price[])。

### PriceName
保存一辆火车中所有票种的名称。

### Train
保存火车的基本信息(id, name, catalog, stationNum, priceNum)和一个保存车站编号的数组。

### Ticket
保存一个订单的基本信息(userID, trainID, num, loc1, loc2, date, kind, catalog)，每个订单还对应一个唯一的编号orderID。

### Time
24小时制保存时间，支持时间的加减运算、大小比较和字符串形式输出。

### String
自定义长度的字符串类，支持常规的字符串操作，如拼接、提取子串等。

## 文件设计
### user\<int, user\>

key：需要查询的用户ID

value：ID对应的用户信息

### sale\<string, train\>

key：需要查询的火车ID

value：ID对应的已发售的列车信息

### nSale\<string, train\>

key：需要查询的火车ID

value：ID对应的未发售的列车信息

### locTrain\<string, train\>

key：查询站点

value：经过该站点的火车信息

### direct\<{string, string, char}, string\>

key：起始站、终点站、列车种类

value：符合要求的火车信息

### trKindTicket\<{string, string, string}, int\>

key：火车ID, 日期, 作为种类

value：符合要求的车票编号

### trDate\<{string, string}, int\>

key：火车ID, 日期

value：符合要求的车票编号

### idTicket\<{int, string, char>, int\>

key：用户ID、日期、列车种类

value：符合要求的车票编号

### infoOrderId\<{int, string, string, string, string, string}, int\>

key：用户ID, 火车ID, 起始站，终点站，日期，列车种类

value：符合要求的订单编号

### orderIdTicket\<int, ticket\>

key：订单编号

value：编号对应的车票信息

### lastId\<string, int\>

key：变量名对应的字符串

value：该变量在程序上一次后的值

### station\<int, Station\>

key：站点编号

value：编号对应的站点信息

### priceName\<string, PriceName\>

key：火车ID

value：ID对应的票种名称



## 树的设计

### insert(Key_Type K, Value_Type V)

在相应位置插入一个(K,V)的元素，成功返回1，否则返回0

### erase(Key_Type K)

在相应位置删除一个关键在为K的元素，成功返回1，否则返回0

### modify(Key_Type K, Value_Type V)

将(K,V)的V转换为输入的值，成功返回1，否则返回0

### find(Key_Type K)

寻找对应关键字K的V，成功返回\<true,V\>，否则返回\<false,V\>

### find_multi(Key_Type K)

寻找对应关键字K的所有V，成功返回包含这些V的vector, 否则返回一个空vector

### clean()

清空整棵树，若文件未创建或未打开返回0，否则成功，返回1



