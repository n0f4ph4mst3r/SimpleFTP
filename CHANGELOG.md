### v0.8.0 (2023-05-03)

*  refactor [8bd5f21c](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/8bd5f21c)

### v0.7.0 (2023-01-07)

##### Refactors

*  refactor [fb674b87](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/fb674b87)

### v0.6.0 (2022-09-06)

##### New Features

*  implement downloading files from the server [d8a477f1](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/d8a477f1)

### v0.5.0 (2022-05-03)

##### Refactors

*  refactor[b6a3c3fb](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/b6a3c3fb)

### v0.4.0 (2022-04-14)


##### New Features

*  client now lists server files in tree view [d7ea6b76](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/d7ea6b76)

### v0.3.0 (2022-02-27)

##### Refactors

* **ClientApp:**
  *  disconnectButton handler cleans wxListBox now [f6fcb14b](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/f6fcb14b)
  *  change connectButton hanler code. [d56f601a](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/d56f601a)
  *  serverlist is wxListBox now [4f28b62a](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/4f28b62a)
  *  change default host [48d38318](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/48d38318)

### v0.2.0 (2022-02-20)

##### Chores

*  add preprocessor directive (_CRT_SECURE_NO_WARNINGS) [d4606eaa](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/d4606eaa)

##### New Features

*  system messages are now displayed in footer [8e99e029](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/8e99e029)

##### Other Changes

*  connection to server implemented using boost::asio [b323addb](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/b323addb)

##### Refactors

* **ClientApp:**
  *  change event handling code [95b4d1ef](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/95b4d1ef)
  *  remove footerListBox and replace it with a footerTextCtrl [9184787d](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/9184787d)
  *  inject elements boost::asio into the event handler [a42e9665](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/a42e9665)
  *  transfer enum to cpp file [a0c3456d](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/a0c3456d)

### v0.1.0 (2022-01-31)

##### Documentation Changes

*  update gitignore [1d58b18e](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/1d58b18e)

##### New Features

*  add the possibility to join the server [b1303c90](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/b1303c90)

##### Other Changes

*  add default value in wxTextCtrls [2af8f537](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/2af8f537)
*  add Boost include and lib files paths [f51cd2f7](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/f51cd2f7)
*  rewrite accessPanel [0eeb8832](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/0eeb8832)

##### Refactors

* **ClientApp:**
  *  add event handlers to change the text in TextCtrls responsible for accessing the server [03ee62ca](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/03ee62ca)
  *  moved widget description to header file [e752027f](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/e752027f)
  *  add event handlers for "Connect" and "Disconnect" buttons [50f2b9a5](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/50f2b9a5)
  *  delete event dirComboBoxOnCreate [9c31927f](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/9c31927f)
*  remove ListBoxs, wxGenericDirCtrls [e222c2b](https://github.com/n0f4ph4mst3r/SimpleFTP/commit/e222c2b)

