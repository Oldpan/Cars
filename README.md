# Cars
华为精英挑战赛的源代码－初赛C++代码 初赛得分 3659

看到大家都开源代码了，我这里也开源一下吧，我们队伍名称为**呀哈哈**(玩过switch的应该清楚什么意思吧哈哈)，西北赛区应该是42(但是系统没有显示，问了负责人，说是我们有一张图提交失败，但是我们这边
是有显示的，总之很奇怪)。

![score](https://github.com/Oldpan/Cars/blob/master/20190401090044.jpg)

陪大家跑了10来天，总的来说收获很多吧，实现了判题器，但是和官方仍然有出入(结果50单位时间以内)。可能一开始的路走错了，刚开始没有
写算法只是撸判题器了，而且前提没有好好地看规则，总是有一些规则疏忽了后期需要再改，耽误了不少时间，总之原因很多~

## 代码执行

程序入口在`main.cpp`将`main.cpp`改为官方提供SDK的入口代码名称即可，代码有很多注释，应该比较好阅读，希望对大家有些帮助吧~

## 我们的教训

有几点教训想在这里总结一下。

### 策略

因为我们实现了判题器花了很多时间(很多坑)，最后算法上没有怎么去研究，只是采用了迪杰斯塔拉最短路径算法，再配合上判题器提供的可以实时获取的动态权重，让车在固定权重和动态权重中搜寻最优的路径
这种做法也是需要调参，但是我们程序执行的时间比较长(有判题器)，所以调参代价稍大。最后还是调参没有调好，差了几百。

### 死锁检测

我实现的判题器虽然尽量和官方一致了，但是有一些情况我这里不会报死锁，官方的会报(我这边报，官方也报)，总之怎么正确检测死锁还是个遗留问题。

### 队伍一定要团队协作
虽然我们队伍是三个人，实际写代码的只有一个，各写各的相当于单干，交流的比较少，最后一天我们队伍另一个人写的代码无法正常运行才使用我这边的代码跑的。当然
我的代码借鉴了我队友的一些。

### 写代码前多看看规则

这个很重要，要不然后期修改的成本太大太麻烦，而且很烦，希望进了复赛的同志们好好看看复赛的规则，争取一步到位，最后成型的代码不要修改太多。

## 后记

总之参加这个比赛收获很多，如果进复赛当然最好，如果没有进体验体验过程也是不错的，希望进了复赛的童鞋们好好加油，发挥自己应有的实力~




