function is_set(cards3) {
  let good: bool = true;
  for (let d: number = 0; d < 4; ++d) {
   const dset = new Set()
   for (let i: number = 0; i < 3; ++i) {
     let m = cards3[i] % 3;
     cards3[i] = cards3[i]/3|0;
     dset.add(m);
   }
   good = good && (dset.size != 2);
  }
  return good;
}

export function find_set(cards) {
  console.log("cheat hints:")
  for (let i: number = 0; i < cards.length; ++i) {
    for (let j: number = i + 1; j < cards.length; ++j) {
      for (let k: number = j + 1; k < cards.length; ++k) {
        if (is_set([cards[i], cards[j], cards[k]])) {
	  console.log("cheat: " + i + " " + j + " " + k);
	}
      }
    }
  }
}
