
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
      create(fn)(arg);
      return thread;
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
  let spawn = dg.thread.spawn;
  let print = dg.print;

  spawn(() => {
    print('hello');
    spawn(() => { print('goodbye'); });
    spawn(() => { print('world'); });
  });
}

