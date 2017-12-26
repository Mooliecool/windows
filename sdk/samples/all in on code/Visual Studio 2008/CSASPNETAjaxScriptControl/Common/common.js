function NewElement(tag, properties) {
    var element = document.createElement(tag);
    for (var propertyName in properties) {
        AssignProperties(element, propertyName, properties[propertyName]);
    }

    return element;
}
function AssignProperties(el, propertyName, propertyValue) {
    if (typeof (propertyValue) === "string" || typeof (propertyValue) === "number")
        el[propertyName] = propertyValue;
    if (typeof (propertyValue) === "object") {
        for (var childpropertyName in propertyValue) {
            AssignProperties(el[propertyName], childpropertyName, propertyValue[childpropertyName]);
        }
    }
}