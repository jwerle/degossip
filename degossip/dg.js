
/**
 * `Global' reference
 */

this.global = this;
GeneratorFunction = (function * () {}).constructor;

/**
 * `degossip' interface
 *
 * @api public
 */

module dg {

  let bind = Function.prototype.bind.bind(Function.prototype.call);
  let shift = bind(Array.prototype.shift);
  let unshift = bind(Array.prototype.unshift);
  let pop = bind(Array.prototype.pop);
  let each = bind(Array.prototype.forEach);

  /**
   * Creates a generator context or wraps
   * a function making it 'yieldable'
   *
   * @api public
   * @param {Function|Generator} fn
   */

  export function $ (fn) {
    var gen = null;

    // wrap non generator function
    if (!(fn instanceof GeneratorFunction)) {
      return function () {
        unshift(arguments, null);
        return fn.bind.apply(fn, arguments);
      }
    }

    // init
    gen = fn();

    // callback loop
    void function next (err, value) {
      if (err) {
        throw err;
        return gen.throw(err);
      }
      var ret = gen.next(value);

      if (!ret.done) {
        ret.value(next);
      }
    }();
  }

  /**
   * Print string to stdout
   *
   * @api public
   * @param {String} string
   */

  export function print (string) {
    string = String(string) + '\n';
    write(stdout, string, string.length);
  }

  /**
   * Print string to stderr
   *
   * @api public
   * @param {String} string
   */

  export function error (string) {
    string = String(string) + '\n';
    write(stderr, string, string.length);
  }

  /**
   * Initializes state
   *
   * @api public
   */

  export function init () {
    globals.init();
    loop.run();
  }

  /**
   * `thread' module
   *
   * @api public
   */

  export module thread {

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

      return arg => th.run(arg);
    }
  }

  /**
   * Globals manager
   *
   * @api private
   */

  module globals {

    /**
     * Sleep for `n' milliseconds
     *
     * @api public
     * @param {Number} ms
     */

    function sleep (ms) {
      ms = Date.now() + ms;
      while (Date.now() < ms) ;
    }

    /**
     * `Console' constructor
     *
     * @api public
     * @param {String} name
     */

    function Console (name) {
      if (!(this instanceof Console)) {
        return new Console(name);
      }
    }

    /**
     * Write output to a `fd'
     *
     * @api private
     * @param {Number} fd
     * @param {Mixed} ...args
     */

    Console.prototype._write = function () {
      let fd = shift(arguments);
      for (var i = 0; i < arguments.length; ++i) {
        let value = arguments[i];
        if ('object' == typeof value) {
          if ('function' == typeof value.toJSON) {
            value = value.toJSON();
          } else if ('function' == typeof value.toString) {
            value = value.toString();
          } else {
            value = JSON.stringify(value);
          }
        }

        value = String(value);
        write(fd, value, value.length);
        write(fd, ' ', 1);
      }

      write(fd, '\n', 1);
    };

    /**
     * Log output to `stdout'
     *
     * @api public
     * @param {Mixed} ...args
     */

    Console.prototype.log = function () {
      unshift(arguments, stdout);
      this._write.apply(this, arguments);
    };

    /**
     * Log output to `stderr'
     *
     * @api public
     * @param {Mixed} ...args
     */

    Console.prototype.error = function () {
      unshift(arguments, stderr);
      this._write.apply(this, arguments);
    };

    /**
     * Log debug output to `stderr'
     *
     * @api public
     * @param {Mixed} ...args
     */

    Console.prototype.debug = function () {
      unshift(arguments, 'debug:');
      unshift(arguments, stderr);
      this._write.apply(this, arguments);
    };

    /**
     * Log info output to `stdout'
     *
     * @api public
     * @param {Mixed} ...args
     */

    Console.prototype.info = function () {
      unshift(arguments, 'info:');
      unshift(arguments, stdout);
      this._write.apply(this, arguments);
    };

    /**
     * Initializes global state
     *
     * @api public
     */

    export function init () {
      global.console = new Console('global');
      global.sleep = sleep;
      global.setTimeout = timers.setTimeout;
      global.clearTimeout = timers.clearTimeout;
      global.setInterval = timers.setInterval;
      global.clearInterval = timers.clearInterval;
    }
  }

  /**
   * Program runner
   *
   * @api private
   */

  module program {

    /**
     * Parses `argv/env' and runs
     * program
     *
     * @api public
     */

    export function run () {
      let argv = ARGV.split('_$$$_').filter(Boolean)
      let to = setTimeout(() => { console.log('timeout') }, 100);
      clearTimeout(to);

      let i = setInterval(() => { console.log('beep'); }, 500);

      setTimeout(function () {
        console.log('b')
      }, 200);
      console.log('ok')
      setTimeout(function () {
        console.log('a')
        setTimeout(function () {
          console.log('c');
          setTimeout(function () {
            console.log('d')
            try {
            } catch (e) { console.error(e) }
            let to = setTimeout(function () {
              console.log('bye')
            }, 1000)
            clearTimeout(to);
          }, 500);
        }, 100);
      }, 100);
    }
  }

  /**
   * Main program loop
   *
   * @api private
   */

  module loop {

    /**
     * Main routine
     *
     * @api private
     */

    let main = null;

    /**
     * Loop task queue
     *
     * @api private
     */

    let queue = [];

    /**
     * Runs the main program loop
     *
     * @api public
     */

    export function run () {
      // prevent duplicate runs
      if (null != main) {
        return;
      }

      // run
      $(function * () {
        yield $(runtime)();
      });
    }

    /**
     * Enqueues task
     *
     * @api public
     * @param {Function} task
     */

    export function enqueue (task) {
      return queue.push(task) - 1;
    }

    /**
     * Cancel queue task at index
     *
     * @api public
     * @param {Number} idx
     */

    export function cancel (idx) {
      return queue.splice(idx, 1);
    }

    /**
     * Dequeues a task
     *
     * @api public
     */

    export function dequeue () {
      for (var i = 0; i < queue.length; ++i) ((task, i) => {
        if (null == task) { return; }
        if (queue.inframe) {
          return queue.length;
        } else if ('function' != typeof task) {
          return queue.length;
        }
        queue.inframe = true;
        task(function (err, done) {
          if (err) { return dg.error(err.stack); }
          if (done) { queue[i] = null; }
          queue.inframe = false;
        });
      })(queue[i], i);
      queue = queue.filter(Boolean);
      return queue.length;
    }

    /**
     * Runtime routine
     *
     * @api private
     */

    function runtime (done) {
      var stack = [];
      var stop = 0;
      do Thread(function () {
        stop = 1
        try {
          program.run();
          while (dequeue()) ;
        } catch (e) {
          dg.error(e.stack);
          return done(e);
        }

        done();
      }).run(); while (1 != stop);
    }
  }

  /**
   * Timer api
   *
   * @api private
   */

  module timers {

    /**
     * Timers hash
     *
     * @api private
     */

    let timers = Object.create(null);

    /**
     * Generates hex id
     *
     * @api private
     */

    function id () {
      return Math.random().toString(16).slice(2);
    }

    /**
     * `Timer' constructor
     *
     * @api private
     * @param {Number} ms
     */

    function Timer (ms) {
      if (!(this instanceof Timer)) {
        return new Timer(ms);
      }

      this.id = id();
      this.ms = 0;
      this.lid = -1;
      this.fns = [];
      this.active = false;
      this.interval = false;
      this.set(ms);
    }

    /**
     * Sets `ms' for timer
     *
     * @api public
     * @param {Number} ms
     */

    Timer.prototype.set = function (ms) {
      this.ms = ms||0;
      return this;
    };

    /**
     * Repeat timer
     *
     * @api public
     */

    Timer.prototype.repeat = function () {
      this.interval = true;
      return this;
    };

    /**
     * Pushes function to timer queue
     *
     * @api public
     * @param {Function} fn
     */

    Timer.prototype.use = function (fn) {
      if ('function' == typeof fn) { this.fns.push(fn); }
      return this;
    };

    /**
     * Clears timer
     *
     * @api publc
     */

    Timer.prototype.clear = function () {
      if (true == this.active) { loop.cancel(this.lid); }
      return this;
    };

    /**
     * Initializes timer
     *
     * @api public
     */

    Timer.prototype.init = function () {
      var stop = Date.now() + this.ms;
      var fns = this.fns.slice();
      var self = this;
      this.active = true;
      this.lid = loop.enqueue(function (next) {
        var err = null;
        if (Date.now() < stop) {
          next(null, false);
        } else while (fns.length) {
          let fn = fns.shift();
          try { fn(); next(null, true); }
          catch (e) { next(e, true); }
          if (self.interval) {
            self.init();
          }
          self.active = false;
        }
      });
      return this;
    };

    /**
     * Run a function `ms' milliseconds
     * later
     *
     * @api public
     * @param {Function} fn
     * @param {Number} ms
     */

    export function setTimeout (fn, ms) {
      let t = Timer(ms).use(fn).init();
      timers[t.id] = t;
      return t;
    }

    /**
     * Clears timeout
     *
     * @api public
     * @param {Timer} timer
     */

    export function clearTimeout (timer) {
      timer.clear();
      delete timers[timer.id];
    }

    /**
     * Initializes an interval of `fn' every
     * `ms'
     *
     * @api public
     * @param {Function} fn
     * @param {Number} ms
     */

    export function setInterval (fn, ms) {
      return setTimeout(fn, ms).repeat();
    }

    /**
     * Clears interval
     *
     * @api public
     * @param {Timer} timer
     */

    export function clearInterval (timer) {
      clearTimeout(timer);
    }
  }
}

/**
 * Initialize degossip
 */

(dg.init());

