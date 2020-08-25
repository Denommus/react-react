open ReactFrp.React;

type signalPair('a) = {
  signal: signal('a),
  setSignal: 'a => unit,
};

let componentFromSignal = (propsToVdom, props) => {
  /* Starts with an empty element. That is, renders nothing */
  let (element, setElement) = React.useState(() => React.null);

  /* I'm now using useRef because it allows me to clean up resources */
  let propsPair = React.useRef(None);
  let watcher = React.useRef(None);

  React.useEffect0(() => {
    let (propsS, propsF) = S.create(props);
    propsPair.current = Some({signal: propsS, setSignal: x => propsF(x)});
    let vdomS = propsToVdom(propsS);
    /* I need to store the watcher in a variable, otherwise it can be gc'd
       and the signal(element) will stop updating the state */
    watcher.current =
      Some(S.map(newElement => setElement(_ => newElement), vdomS));
    Some(
      () => {
        /* Cleaning up resources */
        Belt.Option.map(propsPair.current, x =>
          S.stop(~strong=true, x.signal)
        )
        |> ignore;
        Belt.Option.map(watcher.current, S.stop(~strong=true)) |> ignore;
        propsPair.current = None;
        watcher.current = None;
      },
    );
  });

  React.useEffect1(() => {
    /* This is where the props passed by parameter become a signal */
    Belt.Option.map(propsPair.current, x => x.setSignal(props)) |> ignore;
    None
  }, [|props|]);

  element;
};

module Utils = {
  let valueFromEvent = ev => ReactEvent.Form.target(ev)##value;
  let emitEventToStream = (streamF, ev) => ev |> valueFromEvent |> streamF;
  let eventFromPromise = promise => {
    open Belt.Result;
    open Js.Promise;
    let (promiseE, promiseF) = E.create();
    promise
    |> then_(x => {
         promiseF(Ok(x));
         promise;
       })
    |> catch(x => {
         promiseF(Error(x));
         promise;
       })
    |> ignore;
    promiseE;
  };
  module Event = {
    let join = ee => E.switch_(E.never, ee);
    let bind = (e, f) => join(E.map(f, e));
  };
};
