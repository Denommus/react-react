open ReactFrp.React;

type signalPair('a) = {
  signal: signal('a),
  setSignal: 'a => unit,
};

let componentFromSignal = (propsToVdom, props) => {
  let (propsPairOption, setPropsS) = React.useState(() => None);
  let (element, setElement) = React.useState(() => React.null);
  let (watcher, setWatcher) = React.useState(() => None);
  switch (propsPairOption) {
  | None =>
    let (propsS, propsF) = S.create(props);
    setPropsS(_ => Some({signal: propsS, setSignal: propsF}));
    ();
  | Some(propsPair) =>
    propsPair.setSignal(props);
    if (watcher == None) {
      setWatcher(_ =>
        Some(
          S.map(
            newElement => setElement(_ => newElement),
            propsToVdom(propsPair.signal),
          ),
        )
      );
      ();
    };
  };
  element;
};

module Utils = {
  let valueFromEvent = ev => (ev |> ReactEvent.Form.target)##value;
  let emitEventToStream = (signalF, ev) => valueFromEvent(ev) |> signalF;
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
