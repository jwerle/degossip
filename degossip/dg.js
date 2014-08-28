
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
  export error
  export thread

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

  /**
   * Print string to stderr
   *
   * @api public
   * @param {String} string
   */

  function error (string) {
    string = String(string) + '\n';
    write(stderr, string, string.length);
  }

  /**
   * `thread' module
   *
   * @api public
   */

  module thread {

    /**
     * Spawn a thread running
     * `fn' with optional `arg'
     *
     * @api public
     * @param {Function} fn
     * @param {Mixed} arg - optional
     */

    export function spawn (fn, arg) {
      return create(fn)(arg);
    }

    /**
     * Creates a function that executes
     * `fn' in a separate thread accepting
     * optional `arg' passed to `fn'
     *
     * @api public
     * @param {Function} fn
     */

    export function create (fn) {
      let th = new Thread((arg) => {
        try { fn.call(th, arg); }
        catch (e) { error(e.stack); }
      });

      return (arg) => { return th.run(arg); };
    }
  }
}

module test {

  export function main () {
    let spawn = dg.thread.spawn;
    let print = dg.print;
    let threads = [];
    const MAX_THREADS = 5;
    const MAX_RECV = 10;

    /*
    spawn(() => {
      for (var i = 0; i < MAX_THREADS; ++i) {
        let th = spawn((n) => {
          var a = {count: 0};
          while (a.count < MAX_RECV) {
            spawn((_) => {
              _.count++;
              print('[thread#'+ n +'('+ _.count +')]: '+ read(stdin, 1024));
              this.exit();
            }, a);
          }
        }, i);
      }
    }).wait();
   */

    spawn(() => {
      let buf = null;
      while ((buf = read(stdin, 1024))) {
        print(buf);
      }
    }).wait();

  }
}

// run this shit!
try { test.main(); }
catch (e) { dg.error(e.stack); }
