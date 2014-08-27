
/**
 * `degossip' interface
 *
 * @api public
 */

module dg {

  /**
   * Exports
   */

  export print

  /**
   * Print string to stdout
   *
   * @api public
   * @param {String} string
   */

  function print (string) {
    string = String(string) + '\n';
    write(stdout, string, string.length);
  }

  print("foo");

  var thread = Thread(function (arg) {
    print("in thread");
    print(arg);
  });


  print("a");
  thread.run("hi");
  print("b");
}

