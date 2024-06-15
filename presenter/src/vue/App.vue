<script setup lang="ts">
import { useSerialPortStore } from "./store/serial";
import { useLessonStore } from "./store/lesson";

import LessonSelector from "./components/LessonSelector.vue";
import Presenter from "./components/Presenter.vue";
import SerialSelector from "./components/SerialSelection.vue";

const serialStore = useSerialPortStore();
const lessonStore = useLessonStore();

// window.onbeforeunload = function(event)
// {
//   try{
//     serialStore.reset();
//     lessonStore.reset();
//   }
//   catch {}
//   return false;
// };
</script>

<template>
  <SerialSelector v-if="!serialStore.isReady"/>
  <LessonSelector v-else-if="!lessonStore.isReady"/>
  <Presenter v-else/>
</template>

<style>

</style>
