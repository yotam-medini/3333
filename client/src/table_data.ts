// import { writable } from 'svelte/store';
// 
// const gcards_initial = [2, 3, 5, 7, 11, 13];
// export const gcards = writable(gcards_initial);
// // export default gcards;
// 
// export const gcards_selected = writable([1, 4]);

export let gcards_selected = [1, 4];

export function gcards_selected_update(newValue) {
  gcards_selected = newValue;
  subscribers.forEach(fn => {
    fn(newValue);
  })
}

let subscribers = [];

export function gcards_selected_subscribe(fn) {
  subscribers.push(fn);
  return function unsubscribe(fn) {
    subscribers.splice(subscribers.indexOf(fn), 1)
  }
}

