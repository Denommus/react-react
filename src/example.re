open ReactFrp.React;

module ShowName = {
  let vdomS = (propsS) =>
    S.map(
      ~eq=(_, _) => false,
      (name) =>
        if (name != "") {
          let greeting = {j|Hello, $name!|j};
          <p> (ReasonReact.stringToElement(greeting)) </p>
        } else {
          <p> (ReasonReact.stringToElement("Hello, unknown person!")) </p>
        },
      propsS
    );
  let component = ReasonReact.reducerComponent("ShowName");
  let make = (~name, _children) => ReactReact.componentFromSignal(component, name, vdomS);
};

module Timer = {
  let initial = ref(0);
  let (timeS, timeF) = S.create(initial^);
  let timeIncrement = () => {
    initial := initial^ + 1;
    timeF(initial^)
  };
  let timerId = Js.Global.setInterval(timeIncrement, 1000);
  let vdomS = (_) =>
    S.map(
      ~eq=(_, _) => false,
      (time) => {
        let timeMessage = time == 1 ? "second" : "seconds";
        let greeting = {j|You've spent $time $timeMessage on this page!|j};
        <div> (ReasonReact.stringToElement(greeting)) </div>
      },
      timeS
    );
  let component = ReasonReact.reducerComponent("Timer");
  let make = (_children) => ReactReact.componentFromSignal(component, (), vdomS);
};

module Input = {
  let (nameS, nameF) = S.create("");
  let vdomS = (_) =>
    S.map(
      ~eq=(_, _) => false,
      (name) =>
        <div>
          <input _type="text" onChange=(ReactReact.emitEventToStream(nameF)) />
          <ShowName name />
          <Timer />
        </div>,
      nameS
    );
  let component = ReasonReact.reducerComponent("Input");
  let make = (_children) => ReactReact.componentFromSignal(component, (), vdomS);
};

ReactDOMRe.renderToElementWithId(<Input />, "index");
