
/**
 * `Global' reference
 */

this.global = this;
GeneratorFunction = (function * () {}).constructor;

Function.prototype.toString =
Function.prototype.toJSON = () => '[Function'+ (this.name ? ': '+ this.name : '') +']';

Array.prototype.toJSON = () => '['+ this.join(', ') + ']';

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
  let slice = bind(Array.prototype.slice);
  let map = bind(Array.prototype.map);
  let concat = bind(Array.prototype.concat);
  let trim = bind(String.prototype.trim);

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
      return () => {
        unshift(arguments, null);
        return fn.bind.apply(fn, arguments);
      }
    }

    // init
    gen = fn();

    // callback loop
    void function next (err, value) {
      var ret = null;
      if (err) { return gen.throw(err); }
      ret = gen.next(value);
      if (!ret.done) { ret.value(next); }
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
          if (null == value) {
            value = 'null'
          } else if ('function' == typeof value.toJSON) {
            value = value.toJSON();
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

      {
        let ctx = new TCPContext();
        let sock = new TCPSocket(ctx, TCPSocket.STREAM);
        let buf = null;
        let id = 0;
        let first = 0;
        sock.bind('tcp://*:8080');

        function compare (a, b) {
          if (null == b || a == null) { return 0; }
          let len = slice(a).length;
          for (var i = 0 ; i < len; ++i) {
            if (a[i] != b[i]) { return 0; }
          }
          return 1;
        }

        function assert (e) {
          if (!e) { throw new Error("assertion error"); }
        }

        function array (a) {
          if (null == a) { return Array(); }
          if (undefined == a.length) { a.length = Object.keys(a).length; }
          return slice(a);
        }

        function recv () {
          let size = 6;
          let msg = Array(size);

          for (var i = 0; i < size; ++i) {
            let buf = sock.read(BUFSIZ);
            msg[i] = array(buf);
          }

          return {
            //id: msg[4],
            id: new Uint8Array(msg[4]),
            buffer: new Uint8Array(msg[3])
          };
        }


        while (1) {
          buf = recv();
          console.log(buf.id)
          sock.write(buf.id, TCPSocket.SEND_MORE);
          sock.write("wooooooo", TCPSocket.SEND_MORE);
          sock.write(buf.id, TCPSocket.SEND_MORE);
          sock.write('', TCPSocket.SEND_MORE);
          console.log(map(buf.buffer, (ch) => String.fromCharCode(ch)).join(''))
        }
        return;

        buf.length = Object.keys(buf).length;
        id.length = Object.keys(id).length;

        id = slice(id);
        buf = slice(buf);

        let uid = new Uint8Array(id);
        let ubuf = new Uint8Array(buf);
        console.log(
          map(ubuf, b => b)
          .filter(Boolean)
          .map(ch => String.fromCharCode(ch))
        )
        //console.log(map(u, b => String.fromCharCode(b)))
      }
      return;


      function listen (addr, done) {
        let ctx = new TCPContext();
        let sock = new TCPSocket(ctx, TCPSocket.STREAM);
        let body = [];
        let to = null;

        // poll
        to = setInterval(function () {
          //for (var i = 0; i < 4; ++i) Thread(function () {
            let buf = null;
            const PEEK = 3;
            try {
              buf = sock.read(1);
              if (null != buf) {
                if (buf.length) { body.push(buf); }
                while (null != (buf = sock.read(1024))) {
                  if (buf.length) {
                    body.push(buf);
                  }
                }
              }
            } catch (e) {
              return done(e, null);
            }
            if (body.length) {
              done(null, body.splice(0, body.length).join(''));
            }
          //}).run()
        });

        // bind
        return {
          socket: sock.bind(addr),
          read: sock.read.bind(sock),
          write: sock.write.bind(sock),
          close: () => {
            clearInterval(to);
            sock.close();
            ctx.destroy();
          }
        }
      }

      $(function * () {
        let n = 0;
        let sock = listen('tcp://*:8080', function (err, buf) {
          if (err) { console.error('err', err); throw err; }
          console.log('s', sock.write("foo\n", TCPSocket.SEND_MORE));
          console.log('b', buf)
          if (++n == 3) { sock.close(); }
        });
      });
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

    export var queue = Object.create(Array.prototype, {
      constructor: {value: function Queue () {} }
    });

    /**
     * Runs the main program loop
     *
     * @api public
     */

    export function run () {
      const MAX_THREAD = 2;
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
      delete queue[idx];
      queue.splice(idx, 1);
      queue = queue.filter(Boolean);
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
          if (err) { return dg.error(err.stack ? err.stack : err); }
          if (done) { delete queue[i]; }
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
      try {
        program.run();
        while (dequeue()) ;
      } catch (e) {
        dg.error(e.stack ? e.stack : e);
        return done(e);
      }
      done();
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
      if (true == this.active) {
        this.active = false;
        this.interval = false;
        loop.cancel(this.lid);
      }
      return this;
    };

    /**
     * Initializes timer
     *
     * @api public
     */

    Timer.prototype.init = function () {
      var stop = Date.now() + this.ms;
      var self = this;
      this.active = true;
      this.lid = loop.enqueue(function (next) {
        var err = null;
        var fns = self.fns.slice();
        if (false == self.active) {
          self.clear();
          next(null, true);
        } if (Date.now() < stop) {
          next(null, false);
        } else if (fns.length) {
          while (fns.length) {
            let fn = fns.shift();
            try { fn(); }
            catch (e) { return next(e, true); }
          }

          if (self.interval) {
            next(null, true);
            self.init();
          } else {
            self.active = false;
            next(null, false);
          }
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

dg.init();

