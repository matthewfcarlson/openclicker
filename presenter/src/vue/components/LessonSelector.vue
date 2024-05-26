<script setup lang="ts">
import { ref } from "vue";

import {useLessonStore} from "../store/lesson";
import {useSerialPortStore} from "../store/serial";
import lessons from "../../lessons/built-in-lessons.json"

const errorMessage = ref("");
const lessonStore = useLessonStore();
// console.log(default_lessons)

const openFile = async () => {
  errorMessage.value = "";
  const errors = await lessonStore.attemptFileLoad();
  if (errors != null) errorMessage.value = errors;
};

const loadLesson = async(lesson:any) => {
  errorMessage.value = "";
  const errors = await lessonStore.attemptLessonLoad(lesson);
  if (errors != null) errorMessage.value = errors;
}

</script>
<template>
  <div style="background-color: beige">
   <div class="container">
      <div class="block">
        <h1 class="title">Load Your Lesson Plan</h1>
      </div>
      <button class="button is-fullwidth my-3" v-for="lesson,lessonName in lessons" @click="loadLesson(lesson)"> {{ lessonName }} </button>
      <button class="button is-fullwidth my-3 is-primary" type="button" @click="openFile()">Load Lesson Plan</button>
      <div v-if="errorMessage && errorMessage.length > 0">ERROR: {{ errorMessage }}</div>
      <button class="button is-danger" @click="useSerialPortStore().reset">Go back</button>
    </div>
  </div>
</template>