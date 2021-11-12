val puts_ = _import "puts" public: string -> int;
fun puts s = (puts_ s; ())

structure Queue:
   sig
      type 'a t

      val new: unit -> 'a t
      val enque: 'a t * 'a -> unit
      val deque: 'a t -> 'a option
      val update: ('a -> 'a) -> ('a -> bool) -> 'a t -> unit
   end =
   struct
      datatype 'a t = T of {front: 'a list ref, back: 'a list ref}

      fun new () = T {front = ref [], back = ref []}

      fun enque (T {back, ...}, x) = back := x :: !back

      fun deque (T {front, back}) =
         case !front of
            [] => (case !back of
                      [] => NONE
                    | l => let val l = rev l
                           in case l of
                              [] => raise Fail "deque"
                            | x :: l => (back := []; front := l; SOME x)
                           end)
          | x :: l => (front := l; SOME x) 
      
      fun update f p (T {front, back}) = let
        fun fm e = if p e then f e else e
      in
        (front := map fm (!front); back := map fm (!back))
      end
   end

structure Thread:
   sig
      type tid
      
      val exit: unit -> 'a
      val run: unit -> unit
      val spawn: (unit -> unit) -> tid
      val yield: unit -> unit

      val getTid: unit -> tid
      val setHandler: (int -> unit) -> unit
      val defaultHandler: int -> unit

      val signal: int -> tid -> unit
      val kill: tid -> unit
      val araise: exn -> tid -> unit
      
      val block: unit -> unit
      val unblock: unit -> unit
   end =
   struct
      open MLton
      open Thread

      datatype greensig = GSIGKILL | GSIGEXN of exn | GSIGUSER of int
      datatype tid = TID of int

      datatype thread = THREAD of {
          tid : tid,
          mt : unit Thread.t option,
          sigHandler : int -> unit,
          sigPend : greensig list
      }

      local
        val tidCtr : int ref = ref 1
      in
        fun freshTid () = let val tid = !tidCtr in (tidCtr := tid+1; TID tid) end
      end

      fun defaultHandler s = ()

      val topLevel: thread option ref = ref NONE

      val curThread: thread option ref = ref (SOME (THREAD {
          tid=TID 0,
          mt=NONE,
          sigHandler=defaultHandler,
          sigPend=[]
        }))

      local
         val threads: thread Queue.t = Queue.new ()
      in
         fun ready (t: thread) : unit =
            Queue.enque(threads, t)
         fun next () : thread =
            case Queue.deque threads of
               NONE => valOf (!topLevel)
             | SOME t => t
         fun updateThread f tid = Queue.update f (fn (THREAD t) => #tid t = tid) threads
      end

      fun unpend (THREAD {tid, mt, sigHandler, sigPend=[]}) = THREAD {tid=tid, mt=mt, sigHandler=sigHandler, sigPend=[]}
        | unpend (THREAD {tid, mt, sigHandler, sigPend=s::ss}) = let
            val act = case s of
              GSIGKILL => (fn () => doSwitch (fn _ => next ())) (*exit*)
            | (GSIGEXN e) => (fn () => raise e)
            | (GSIGUSER s) => (fn () => sigHandler s)
            val pt = SOME (Thread.prepend (valOf mt, act))
          in
            THREAD {tid=tid, mt=pt, sigHandler=sigHandler, sigPend=ss}
          end
      
      and doSwitch f = switch (fn k => let
        val THREAD {tid, mt, sigHandler, sigPend} = valOf (!curThread)
        val curT = THREAD {tid=tid, mt=SOME k, sigHandler=sigHandler, sigPend=sigPend}
        val () = curThread := NONE
        val THREAD {tid, mt, sigHandler, sigPend} = unpend (f curT)
        val () = curThread := SOME (THREAD {tid=tid, mt=NONE, sigHandler=sigHandler, sigPend=sigPend})
      in
        Thread.prepare (valOf mt, ())
      end)

      fun 'a exit (): 'a = (doSwitch (fn _ => next ()); raise (Fail "I'm dead?"))

      fun new (f: unit -> unit): thread = let
        val k = SOME (
         (Thread.new (fn () => ((f () handle _ => exit ())
                                ; exit ()))))
        val tid = freshTid ()
      in
        THREAD {tid=tid, mt=k, sigHandler=defaultHandler, sigPend=[]}
      end

      fun schedule t = (ready t; next ())

      fun yield (): unit = doSwitch schedule

      fun spawn f = let val THREAD t = new f in (ready (THREAD t); #tid t) end

      fun run (): unit = (
        doSwitch (fn t => (topLevel := SOME t; next ())); 
        topLevel := NONE
      )


      fun getTid () = let val THREAD t = valOf (!curThread) in #tid t end

      fun setHandler (h: int -> unit) = let
        val THREAD {tid, mt, sigHandler, sigPend} = valOf (!curThread)
      in
        curThread := SOME (THREAD {tid=tid, mt=mt, sigHandler=h, sigPend=sigPend})
      end

      fun pend gs (THREAD {tid, mt, sigHandler, sigPend}) = THREAD {
        tid=tid,
        mt=mt,
        sigHandler=sigHandler,
        sigPend=sigPend @ [gs]
      }

      fun sendIt gs tid = if tid = getTid () then () else updateThread (pend gs) tid 

      fun signal s tid = sendIt (GSIGUSER s) tid
      fun kill tid = sendIt (GSIGKILL) tid
      fun araise e tid = sendIt (GSIGEXN e) tid

      fun block () = ()
      fun unblock () = ()
   end

exception Stop

fun b () = (Thread.yield (puts "I'm thread B!"); b ()) handle Stop => (puts "I'll stop now.")

fun a () = let
  val tidB = Thread.spawn b
  fun loop 0 = () 
    | loop n = (Thread.yield (puts "I'm thread A!"); loop (n-1))
in
  (
    loop 3;
    puts "Telling B to stop.";
    Thread.araise Stop tidB;
    Thread.exit ()
  )
end

val tidA = Thread.spawn a

val _ = Thread.run ()
val _ = puts "Success!"

