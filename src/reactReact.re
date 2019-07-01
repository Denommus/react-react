open ReactFrp.React;

type signalPair('a) = {
  signal: signal('a),
  setSignal: 'a => unit,
};

let componentFromSignal = (propsToVdom, props) => {
  let (element, setElement) = React.useState(() => React.null);
  let (propsPair, _) =
    React.useState(() => {
      let (propsS, propsF) = S.create(props);
      {signal: propsS, setSignal: propsF};
    });
  propsPair.setSignal(props);
  let (_watcher, _) =
    React.useState(() => {
      let vdomS = propsToVdom(propsPair.signal);
      S.map(newElement => setElement(_ => newElement), vdomS);
    });
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
