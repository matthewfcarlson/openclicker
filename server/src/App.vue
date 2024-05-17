<script setup lang="ts">
import { useSerialPortStore } from "./store/serial";
import { useLessonStore } from "./store/lesson";

import LessonSelector from "./components/LessonSelector.vue";
import Presenter from "./components/Presenter.vue";
import SerialSelector from "./components/SerialSelection.vue";

const serialStore = useSerialPortStore();
const lessonStore = useLessonStore();
</script>

<template>
  <SerialSelector v-if="!serialStore.isReady"/>
  <LessonSelector v-else-if="!lessonStore.isReady"/>
  <Presenter v-else/>
</template>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
img {
  margin: 5px;
}
.plugins {
  font-size: 20px;
  font-weight: bold;
  margin-top: 20px;
}
</style>
