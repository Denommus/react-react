open ReactFrp.React;

type action('a) =
  | Tick('a);

type state('a) = {
  propsF: ref(option(((~step: step=?, 'a) => unit))),
  subscription: ref(option(signal(unit))),
  vdom: ReasonReact.reactElement
};

let componentFromSignal =
    (
      ~propsEq=(==),
      component:
        ReasonReact.componentSpec(
          state('a),
          ReasonReact.stateless,
          ReasonReact.noRetainedProps,
          ReasonReact.noRetainedProps,
          action(ReasonReact.reactElement)
        ),
      props,
      propsToVdom
    ) => {
  ...component,
  initialState: () => {
    propsF: ref(None),
    subscription: ref(None),
    vdom: ReasonReact.arrayToElement([||])
  },
  reducer: (action, state) =>
    switch action {
    | Tick(x) => ReasonReact.Update({...state, vdom: x})
    },
  willUpdate: (oldAndNewSelf) =>
    switch oldAndNewSelf.newSelf.state.propsF^ {
    | Some(f) => f(props)
    | None => ()
    },
  didMount: ({state, reduce}) => {
    let (propsS, propsF) = S.create(~eq=propsEq, props);
    state.propsF := Some(propsF);
    let vdomS = propsToVdom(propsS);
    state.subscription := Some(S.map((newVdom) => reduce((_event) => Tick(newVdom), ()), vdomS));
    ReasonReact.NoUpdate
  },
  willUnmount: ({state}) => {
    state.subscription := None;
    state.propsF := None;
    ()
  },
  render: ({state}) => state.vdom
};

let valueFromEvent = (ev) => (ev |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj)##value;

let emitEventToStream = (signalF, ev) => valueFromEvent(ev) |> signalF;

module Utils = {
  let eventFromPromise = (promise) => {
    open Js.Result;
    open Js.Promise;
    let (promiseE, promiseF) = E.create();
    promise
    |> then_(
         (x) => {
           promiseF(Ok(x));
           promise
         }
       )
    |> catch(
         (x) => {
           promiseF(Error(x));
           promise
         }
       )
    |> ignore;
    promiseE
  };
  let eventJoin = (ee) => E.switch_(E.never, ee);
  let eventBind = (e, f) => eventJoin(E.map(f, e));
};
