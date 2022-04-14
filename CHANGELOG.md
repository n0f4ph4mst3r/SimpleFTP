### v0.4.0 (2022-04-14)

##### New Features

*  client now lists server files in tree view [d7ea6b76](https://github.com/n0f4ph4mst3r/FTP-Client/d7ea6b76)

### v0.3.0 (2022-02-27)

##### Refactors

* **ClientApp:**
  *  disconnectButton handler cleans wxListBox now [f6fcb14b](https://github.com/n0f4ph4mst3r/FTP-Client/f6fcb14b)
  *  change connectButton hanler code. [d56f601a](https://github.com/n0f4ph4mst3r/FTP-Client/d56f601a)
  *  serverlist is wxListBox now [4f28b62a](https://github.com/n0f4ph4mst3r/FTP-Client/4f28b62a)
  *  change default host [48d38318](https://github.com/n0f4ph4mst3r/FTP-Client/48d38318)

### v0.2.0 (2022-02-20)

##### Chores

*  add preprocessor directive (_CRT_SECURE_NO_WARNINGS) [d4606eaa](https://github.com/n0f4ph4mst3r/FTP-Client/d4606eaa)

##### New Features

*  system messages are now displayed in footer [8e99e029](https://github.com/n0f4ph4mst3r/FTP-Client/8e99e029)

##### Other Changes

*  connection to server implemented using boost::asio [b323addb](https://github.com/n0f4ph4mst3r/FTP-Client/b323addb)

##### Refactors

* **ClientApp:**
  *  change event handling code [95b4d1ef](https://github.com/n0f4ph4mst3r/FTP-Client/95b4d1ef)
  *  remove footerListBox and replace it with a footerTextCtrl [9184787d](https://github.com/n0f4ph4mst3r/FTP-Client/9184787d)
  *  inject elements boost::asio into the event handler [a42e9665](https://github.com/n0f4ph4mst3r/FTP-Client/a42e9665)
  *  transfer enum to cpp file [a0c3456d](https://github.com/n0f4ph4mst3r/FTP-Client/a0c3456d)

### v0.1.0 (2022-01-31)

##### Documentation Changes

*  update gitignore [1d58b18e](https://github.com/n0f4ph4mst3r/FTP-Client/1d58b18e)

##### New Features

*  add the possibility to join the server [b1303c90](https://github.com/n0f4ph4mst3r/FTP-Client/b1303c90)

##### Other Changes

*  add default value in wxTextCtrls [2af8f537](https://github.com/n0f4ph4mst3r/FTP-Client/2af8f537)
*  add Boost include and lib files paths [f51cd2f7](https://github.com/n0f4ph4mst3r/FTP-Client/f51cd2f7)
*  rewrite accessPanel [0eeb8832](https://github.com/n0f4ph4mst3r/FTP-Client/0eeb8832)

##### Refactors

* **ClientApp:**
  *  add event handlers to change the text in TextCtrls responsible for accessing the server [03ee62ca](https://github.com/n0f4ph4mst3r/FTP-Client/03ee62ca)
  *  moved widget description to header file [e752027f](https://github.com/n0f4ph4mst3r/FTP-Client/e752027f)
  *  add event handlers for "Connect" and "Disconnect" buttons [50f2b9a5](https://github.com/n0f4ph4mst3r/FTP-Client/50f2b9a5)
  *  delete event dirComboBoxOnCreate [9c31927f](https://github.com/n0f4ph4mst3r/FTP-Client/9c31927f)
*  remove ListBoxs, wxGenericDirCtrls [e222c2b](https://github.com/n0f4ph4mst3r/FTP-Client/e222c2b)

