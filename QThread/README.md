# 记录Qt多线程的其中一种方法 moveToThread
 通过测试，在mainwidow.cpp使用上面的代码的时候，由于my调用了movetothread函数，那么它所有的槽函数都是执行在新开辟的线程里面。
   如果去掉moveToThread函数，那么所有的函数都将执行在gui线程里面。

   同时为了测试connect的第五个参数，在connect的时候可以将Qt::QueuedConnection修改为Qt::DirectConnection，这样所有的槽函数也将在主线程里面执行。

     最后要注意的是，如果上面connect的时候连接的是this的onXXXXXX槽函数再来调用的my的槽函数的话，那么这些槽函数也将执行在onXXXXX槽函数所在的线程，这里是主线程。

     通过上面的测试，我们在使用线程的时候，就可以将一个类派生自QObject，然后实现所有的signal/slot，然后通过调用movetothread函数来使他们执行在新的线程里面，而不是每次都要重新派生QThread，并且派生QThread函数的另外一个不好的地方是只有run函数内部的代码才会执行在新线程里面，相比起来，派生QObject并使用movetothread函数更具有灵活性。
