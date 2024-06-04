<script setup lang="ts">
import { ref, computed, onMounted } from "vue";

import {useLessonStore} from "../store/lesson";
import { storeToRefs } from "pinia";

import SectionComp from "./Lesson/SectionComp.vue";
import IntroComp from "./Lesson/IntroComp.vue";
import ErrorComp from "./Lesson/ErrorComp.vue";
import LoadingComp from "./Lesson/LoadingComp.vue";
import { LessonStateNames } from "../../common/LessonStates";

const ipcRenderer = window.require('electron').ipcRenderer;
const msg = ref("");

function keyCallback(ev:Event) {
  console.log("CALLBACK");
  console.log(ev);
  return ev;
}

function sendMessage() {
  console.log("Sending message", msg.value);
  ipcRenderer.invoke('message-from-presenter', msg.value);
}

onMounted(() => {
  console.log("mounted");
  window.addEventListener('onkeydown', keyCallback)

  ipcRenderer.on('message-to-presenter', (x, msg)=> {
    console.log("To Presenter", msg);
    // TODO: decode these messages as needed
  })
});


const store = useLessonStore();
const currentComponent = computed(() => {
  const currentState = store.currentLessonState
  if (currentState ==null) {
    return LoadingComp;
  }
  switch (currentState.type) {
    case LessonStateNames.Introduction:
      return IntroComp;
    case LessonStateNames.Section:
      return SectionComp
  }
  return ErrorComp;
})

</script>
<template>
  <component :is="currentComponent"/>
  <pre>Current Lesson: {{store.currentLessonState}}</pre>
  <button :disabled="!store.canRewind" class="button" @click="store.rewindLessonState">Prev</button>
  <button :disabled="!store.canAdvance" class="button" @click="store.advanceLessonState">Next</button>
  <input v-model="msg"/>
  <button @click="sendMessage">Send</button>
</template>